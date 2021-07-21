//
// Created by andrea on 21/07/21.
//

#ifndef SYSTEM_PROGRAMMING_PROCESS_TREE_H
#define SYSTEM_PROGRAMMING_PROCESS_TREE_H


#include <vector>
#include <map>
#include <string>
#include <any>
#include <memory>
#include <set>

using namespace std;

struct process;

typedef shared_ptr<process> process_p;

struct process {
    int pid;
    u_int exec_time = 0;
    map<string, string> status; // key ->  value in /proc/nnn/status
    set<string> files; // open files in /proc/nnn/fd
    set<string> map_files; // mapped files in /proc/nnn/map_files
    vector<process_p> children;

    process(int pid) : pid(pid) {};

    void clear() {
        status.clear();
        files.clear();
        map_files.clear();
        children.clear();
    }
};

class ProcessManager {

    map<int, process_p> processes;  // pid -> process

    vector<process_p> get_subtree(int root);

public:
    void update(const string &infile);

    // example methods
    void list_processes_by_nfiles(int root);

    void list_files_by_nprocesses(int root);

    void print_tree(int root);

};

#endif //SYSTEM_PROGRAMMING_PROCESS_TREE_H
