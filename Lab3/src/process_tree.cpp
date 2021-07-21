//
// Created by andrea on 21/07/21.
//

#include "../lib/process_tree.h"
#include <iostream>
#include <fstream>
#include <map>
#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <stack>


enum section {
    status, stat, fd, map_files
};

std::map<string, section> secmap = {
        {"status",    status},
        {"stat",      stat},
        {"fd",        fd},
        {"map_files", map_files},
};


vector<string> tokenize(const string &s, char delim) {
    size_t start;
    size_t end = 0;
    vector<string> v;
    while ((start = s.find_last_not_of(delim, end)) != string::npos) {
        end = s.find(delim, start);
        v.push_back(s.substr(start, end - start));
    }
    return v;
}

void ProcessManager::update(const string &infile) {
    ifstream is{infile};
    string line;
    section sec = status;

    set<int> missing_pids;
    for (auto &[k, v]: processes) missing_pids.insert(k);

    process_p cprocess;

    // getline(is, line); // read first line "---"
    while (getline(is, line)) {

        // process split header
        if (line.find("---") == 0) {
            // next line is the file name
            getline(is, line);
            vector<string> toks;
            boost::split(toks, line, boost::is_any_of("/"));

            if (toks.size() == 3) {
                int pid = stoi(toks.back());
                auto it = processes.find(pid);
                if (it != processes.end()) {
                    // process already present
                    cprocess = it->second;
                    cprocess->clear();
                    missing_pids.erase(pid);
                } else {
                    cprocess = make_shared<process>(pid);
                    processes[pid] = cprocess;
                }

            } else {
                sec = secmap[toks.back()];
            }
            continue;
        }

        switch (sec) {
            case status: {
                // split key: val
                int delpos = line.find(":");
                string k = line.substr(0, delpos);
                string v = line.substr(delpos + 1);
                boost::algorithm::trim(v);
                cprocess->status[k] = v;
                break;
            }
            case stat: {
                // parse stat: get execution time
                break;
            }
            case fd:
            case map_files: {
                int arpos = line.find("->");
                if (arpos == string::npos) continue;
                string file = line.substr(arpos + 2);
                boost::algorithm::trim(file);
                if (sec == fd)
                    cprocess->files.insert(file);
                else
                    cprocess->map_files.insert(file);
                break;
            }
        }
    }
    // remove ended processes
    for (int pid: missing_pids) processes.erase(pid);

    // build parent / child relations
    // replace "virtual" root
    processes[0] = make_shared<process>(0);
    for (auto &[pid, p]: processes) {
        if (pid == 0) continue;
        string parent_id = p->status["PPid"];
        try {

            int _pid = stoi(parent_id);
            auto it = processes.find(_pid);
            if (it != processes.end())
                it->second->children.push_back(p);
        } catch (invalid_argument) {
            // just ignore missing PPids
        }
    }

}

vector<process_p> ProcessManager::get_subtree(int root) {
    vector<process_p> sel_processes;
    stack<process_p> visit;
    visit.push(processes[root]);
    while (!visit.empty()) {
        process_p p = visit.top();
        visit.pop();
        sel_processes.push_back(p);
        for (auto px: p->children) visit.push(px);
    }
    return sel_processes;
}


void ProcessManager::list_processes_by_nfiles(int root) {
    vector<process_p> v = get_subtree(root);
    sort(v.begin(), v.end(), [](process_p a, process_p b) -> bool { return a->files.size() > b->files.size(); });

    int i = 0;
    for (auto p: v) {
        cout << "[" << ++i << "] " << p->status["Name"] << ": " << p->files.size() << endl;
    }
};


void ProcessManager::list_files_by_nprocesses(int root) {

    vector<process_p> v = get_subtree(root);

    // extract files and count processes;
    map<string, int> fmap;
    for (auto p: v) {
        for (string f: p->files) {
            fmap[f]++; // missing
        }
    }

    // transform into an array so that we can sort
    typedef pair<int, string> val_str;
    vector<val_str> files;
    for (auto &[k, v]: fmap) {
        files.push_back(val_str(v, k));
    }
    sort(files.begin(), files.end(), [](const val_str &a, const val_str &b) -> bool {
        return a.first > b.first;
    });

    int i = 0;
    for (auto &[count, fname]: files) {
        cout << "[" << ++i << "] " << fname << " " << count << endl;
    }

}

void ProcessManager::print_tree(int root) {
    // depth first visit
    typedef pair<int, process_p> node_depth;
    stack<node_depth> visit;
    visit.push(node_depth(0, processes[root]));
    while (!visit.empty()) {
        auto &[depth, p] = visit.top();
        visit.pop();
        cout << string(depth * 2, ' ') << p->pid << " " << p->status["Name"] << endl;
        vector<process_p> &children = p->children;
        auto it = children.rbegin();
        while (it != children.rend()) { // append children in reverse order to preserve visit order
            visit.push(node_depth(depth + 1, *it));
            it++;
        }
    }

}