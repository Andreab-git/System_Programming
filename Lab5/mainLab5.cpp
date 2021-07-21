//
// Created by andrea on 21/07/21.
//

#include <iostream>
#include <string>
#include <vector>
#include <list>
#include <random>
#include <mutex>
#include <fstream>
#include <thread>
#include <queue>
#include <optional>
#include <sstream>
#include <condition_variable>
#include "semaphore.h"
#include "myguard.h"

using namespace std;


void es1_solution() {
    mutex m;

    thread t1([&m](){
        mutex foo;
        MyGuard guard(m);
        MyGuard fooguard(foo); // to test move
        fooguard = move(guard);
        cout<<"t1 enter"<<endl;
        this_thread::sleep_for(chrono::seconds(3));
        cout<<"t1 exit"<<endl;
    });

    thread t2([&m](){
        // make t1 acquire lock first
        this_thread::sleep_for(chrono::seconds(1));
        MyGuard guard(m);
        cout<<"t2 enter"<<endl;
    });

    t1.join();
    t2.join();
}


// linux only solution (without condition variabile)
template <typename T>
class WaitForValue {
    mutex val_avail;
    mutex val_set;
    T val;

public:
    WaitForValue(){
        val_avail.lock();
    }

    T wait(){
        val_avail.lock();
        T copy = val;
        val_set.unlock();
        return  copy;
    };

    void signal(T val){
        val_set.lock();
        this->val = val;
        val_avail.unlock();
    };
};

void es2_solution() {
    WaitForValue<int> wfr;
    long sum = 0;

    thread t1([&wfr](){
        for(int i=0; i<1000000; i++){
            wfr.signal(i);
        }
        // end condition
        wfr.signal(-1);
    });

    thread t2([&wfr, &sum](){
        while(true){
            int cval = wfr.wait();
            if(cval<0) break;
            sum += cval%43;
        }
    });

    t1.join();
    t2.join();
    cout<<"sum: "<<sum<<endl;

}

// with condition variable;
template <class T>
class WaitForValue2 {
    mutex m;
    condition_variable cv;
    T val;
    bool ready = false;

public:

    T wait() {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [this](){return this->ready;});
        ready = false;
        cv.notify_one();
        return val;
    }

    void signal(T v) {
        unique_lock<mutex> lk(m);
        cv.wait(lk, [this](){ return !this->ready;});
        val = v;
        ready = true;
        val = v;
        cv.notify_one();
    }

};

void es2_solution2(){

    WaitForValue2<int> wfr;
    long sum = 0;

    clock_t t0 = clock();

    thread t1([&wfr](){
        for(int i=0; i<1000000; i++){
            wfr.signal(i);
        }
        // end condition
        wfr.signal(-1);
    });

    thread t2([&wfr, &sum](){
        while(true){
            int cval = wfr.wait();
            if(cval<0) break;
            sum += cval%43;
        }
    });

    t1.join();
    t2.join();

    cout<<"sum: "<<sum<<endl;
    cout<<"time (concurrent): "<<clock()-t0<<endl;

    // serial solution
    t0 = clock();
    vector<int> counts;
    for(int i=0; i<1000000; i++) counts.push_back(i);
    sum = 0;
    for(auto c: counts) sum += c%43;

    cout<<"sum: "<<sum<<endl;
    cout<<"time (serial): "<<clock()-t0<<endl;
}


void es3_solution(){
    string target = "amet";
    CountingSemaphore s(4);
    vector<int> matches; // store results
    mutex res_m;
    mutex line_m; // sync file read
    int simul = 0;

    // naive solution: read file in each thread (poor speed, since we must sync the input stream:
    // you could build a blocking queue and read the file in the main thread),
    ifstream ifs("/tmp/loremipsum.txt");
    int lineno=0;

    auto find = [&s, target, &res_m, &matches, &line_m, &lineno, &ifs, &simul](){

        while(true){
            s.acquire(); // enter only if less than 4 threads are in
            string line;
            int _lineno;
            { // protect file read
                lock_guard<mutex> lk(line_m);
                if(!getline(ifs, line)){
                    s.release();
                    break;
                }
                _lineno = lineno++;
            }

            if(line.find(target)!=string::npos) {
                // protect results
                lock_guard<mutex> lk(res_m);
                matches.push_back(_lineno);
            }
            s.release();
        }
    };

    vector<thread> threads;
    for(int i=0; i<10; i++){
        threads.push_back(thread(find));
    }
    for(auto &t: threads){
        t.join();
    }

    for(auto i: matches){
        cout<<"line "<<i<<" matched"<<endl;
    }
}

#define N 10
#define MAX_DELAY 10

void es4_solution(){

    CountingSemaphore sem(0);
    queue<optional<pair<string, string>>> taskq;
    mutex task_m;
    time_t t0 = time(nullptr);

    auto prodf = [t0, &sem, &taskq, &task_m]() { // simulate producer
        time_t elapsed = 0;
        for(int i=0; i<N; i++) {
            elapsed += rand()%MAX_DELAY;
            // calculte delay as difference from current time o avoid skews
            time_t wait = max((t0 + elapsed) - time(nullptr), 0L);
            this_thread::sleep_for(chrono::seconds(wait));
            {
                lock_guard<mutex> lk(task_m);
                ostringstream oss1, oss2;
                oss1 << "source [th:" << this_thread::get_id() << " - i:" << i << " - t:" << elapsed << "]";
                taskq.push(pair<string, string>(oss1.str(), "dest "));
            }
            sem.release();
        }
    };

    auto consf = [&sem, &task_m, &taskq](){
        while(true) {
            sem.acquire(); // make sure there is at least one item in the queue
            optional<pair<string, string>> task;
            {
                lock_guard<mutex> lk();
                task = taskq.front();
                taskq.pop();
            }
            if(task == nullopt) break;
            cout<<"moving: "<<task.value().first<<" -> "<<task.value().second<<endl;
        }
    };

    vector<thread> prods;
    for(int i=0; i<10; i++) prods.push_back(thread(prodf));
    thread cons(consf);

    for(auto &t: prods) t.join();
    {
        // all producer ended, push terminal condition
        lock_guard<mutex> lk(task_m);
        taskq.push(nullopt);
    }
    sem.release();
    cons.join();
}

// solution with condition variable
void es4_solution2() {
    queue<optional<pair<string, string>>> taskq;
    condition_variable qcv;
    mutex task_m;
    time_t t0 = time(nullptr);

    auto prodf = [t0, &qcv, &taskq, &task_m]() { // simulate producer
        time_t elapsed = 0;
        for(int i=0; i<N; i++) {
            elapsed += rand()%MAX_DELAY;
            // calculate delay as difference from current time o avoid skews
            time_t wait = max((t0 + elapsed) - time(nullptr), 0L);
            this_thread::sleep_for(chrono::seconds(wait));
            {
                lock_guard<mutex> lk(task_m);
                ostringstream oss1;
                oss1 << "source [th:" << this_thread::get_id() << " - i:" << i << " - t:" << elapsed << "]";
                taskq.push(pair<string, string>(oss1.str(), "dest "));
            }
            qcv.notify_all();
        }
    };

    auto consf = [&qcv, &task_m, &taskq](){
        while(true) {
            unique_lock<mutex> lk(task_m);
            qcv.wait(lk, [&taskq]() -> bool {
                return taskq.size()>0;
            });
            auto task = taskq.front();
            taskq.pop();
            lk.unlock(); // we may unlock early, before performing the task
            if(task == nullopt) break;
            // simulate worker
            cout<<"moving: "<<task.value().first<<" -> "<<task.value().second<<endl;
        }
    };

    vector<thread> prods;
    for(int i=0; i<10; i++) prods.push_back(thread(prodf));
    thread cons(consf);

    for(auto &t: prods) t.join();
    {
        // all producer ended, push terminal condition
        lock_guard<mutex> lk(task_m);
        taskq.push(nullopt);
        qcv.notify_all();
    }

    cons.join();

}

int main() {
    es2_solution2();
    return 0;
}
