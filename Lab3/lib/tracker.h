#ifndef SYSTEM_PROGRAMMING_TRACKER_H
#define SYSTEM_PROGRAMMING_TRACKER_H

#include <cstdlib>
#include "logger_msgs.h"

template <typename D>
class Tracker {
    static u_int count;

public:
    Tracker() {count++;}
    ~Tracker() {
        LOG2("Deconstructor for", (void*) this);
        count--;
    }

    static u_int refs() { return count; }
};


#endif //SYSTEM_PROGRAMMING_TRACKER_H
