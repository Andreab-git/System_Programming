#ifndef SYSTEM_PROGRAMMING_TRACKER_H
#define SYSTEM_PROGRAMMING_TRACKER_H

#include <cstdlib>

template <typename D>
class Tracker {
    static u_int count;

public:
    Tracker() {count++;}
    ~Tracker() {count--;}

    static u_int refs() { return count; }
};


#endif //SYSTEM_PROGRAMMING_TRACKER_H
