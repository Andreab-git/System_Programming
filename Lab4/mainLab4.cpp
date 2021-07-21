//
// Created by andrea on 21/07/21.
//

#include <iostream>
#include <functional>
#include <unistd.h>
#include <limits.h>
#include <sys/wait.h>
#include <vector>
#include <queue>
#include <map>

using namespace std;

void error(const string &msg) {
    cerr << msg << endl;
    exit(1);
}


// it works with any basic type and struct without pointers
// sizeof must return the size of the object and data must be packed inside the object
template<typename T>
T runInChild(function<T(void)> fun) {

    int respipe[2];
    if (pipe(respipe) < 0)
        error("can't open pipe");

    int pid = fork();
    if (pid < 0) error("can't fork");
    else if (pid > 0) {
        close(respipe[1]);
        T res;
        read(respipe[0], &res, sizeof(T));
        wait(nullptr);
        return res;
    } else {
        close(respipe[0]);
        // run child;
        T res = fun();
        write(respipe[1], &res, sizeof(res));
        // child must exit to avoid returning anything to the caller
        exit(0);
    }

}


void test_es1() {

    int v = 10;
    int res = runInChild<int>([v]() -> int {
        return v * 2;
    });
    cout << "Res: " << res << endl;

}

// simulating reading a new task file
typedef pair<int, int> range;
typedef pair<int, range> task;

vector<task> task_gen() {
    int n = rand() % 10; // 0-9 random tasks;
    vector<task> tasks;  // simulate reading of
    for (int i = 0; i < n; i++) {
        int stop = rand() % 100; // 1/100th stop probability
        if (stop == 0) {
            tasks.push_back(task(0, range(0, 0)));
            break;
        }

        // range between 0 - 2000;
        int start = rand() % 1000;
        int end = start + rand() % 1000;
        tasks.push_back(task(rand() % 10, range(start, end)));
    }
    return tasks;
}

// used to send back results
struct message_block {
    int worker_id;
    int start; // range start
    int end; // range end
    int frames; // number of frames in this message
    int cframe; // current frame
    int nresults; // number of valid results
    int results[100]; // block of results (sizeof(message_block) must be less than PIPE_BUF

    message_block() {};

    message_block(int wid, int start, int end, int frames, int cframe) :
            worker_id(wid), start(start), end(end), frames(frames), cframe(cframe), nresults(0) {
    };

    void push(int res) {
        if (nresults == 100) error("results overflow");
        results[nresults++] = res;
    }
};


[[noreturn]] void esercizio_2(int n) {

    bool isTerm = false;
    auto cmp = [](const task &left, const task &right) { return left.first > right.first; };

    // A priority queue is a container adaptor that provides constant time lookup of the largest (by default) element,
    // at the expense of logarithmic insertion and extraction.
    priority_queue<task, vector<task>, decltype(cmp)> q(cmp);
    vector<bool> free_workers(n);
    for (int i = 0; i < n; i++) free_workers[i] = true;

    // start children and make them work
    int pipes[n + 1][2]; // pipes[n] is the return channel
    for (int i = 0; i <= n; i++) {
        if (pipe(pipes[i]) < 0) error("can't create pipe");
    }

    for (int i = 0; i < n; i++) {
        int pid = fork();
        if (pid < 0)
            error("can't fork");
        else if (pid == 0) {
            // worker code
            close(pipes[i][1]);
            close(pipes[n][0]);
            while (true) {
                range r;
                read(pipes[i][0], &r, sizeof(range));
                // cout<<"child "<<i<<": "<<r.first<<" "<<r.second<<endl;

                // add here code to calculate prime numbers.
                sleep(rand() % 3); // simulate some work

                // send more than one block if there are more than 100 results
                // simulate three blocks answers
                for (int j = 0; j < 3; j++) {
                    message_block answer(i, r.first, r.second, 3, j);
                    for (int i = 0; i < 10; i++) answer.push(42 * j);
                    write(pipes[n][1], &answer, sizeof(message_block));
                }
            }
        }

    }

    int pending_results = 0;

    // continue until: no termination received, there are tasks to be scheduled, and pending results
    while (!isTerm || !q.empty() || pending_results > 0) {

        if (!isTerm) { // check for new tasks
            for (auto t : task_gen()) {
                auto[p, r] = t;
                if (p == 0 && r.first == 0 && r.second == 0) {
                    cout << "end received" << endl;
                    isTerm = true;
                } else {
                    q.push(t);
                }
            }
        }

        // assign all the possible tasks to the workers
        while (!q.empty()) {
            int idx;
            for (idx = 0; idx < free_workers.size(); idx++) {
                if (free_workers[idx]) {
                    free_workers[idx] = false;
                    break;
                }
            }
            if (idx == n) break; // no more free workers;
            task t = q.top();
            q.pop();
            pending_results++;
            write(pipes[idx][1], &t.second, sizeof(range));
        }

        // read all possible answers (blocks can be multiplexed, so we must keep them until all blocks are received)
        map<int, vector<message_block>> blocks;
        while (true) {
            message_block block;
            read(pipes[n][0], &block, sizeof(message_block));
            blocks[block.worker_id].push_back(block);

            if (block.cframe == block.frames - 1) { // last block of a result
                pending_results--;
                free_workers[block.worker_id] = true;
                cout << "result for " << block.start << " - " << block.end << ":";
                for (auto cblock: blocks[block.worker_id]) {
                    for (int i = 0; i < cblock.nresults; i++) {
                        cout << " " << (cblock.results[i]);
                    }
                }
                cout << endl;
                blocks.erase(block.worker_id);
            }

            if (blocks.empty()) break; // no pending blocks
        }

    }

    // we don't need to send a isTerm condition and wait since we have no pending results

}


int main() {
    cout << "PPIPE BUF: " << PIPE_BUF << endl;
    esercizio_2(5);
    return 0;
}
