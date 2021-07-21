#ifndef SYSTEM_PROGRAMMING_TRACKER_H
#define SYSTEM_PROGRAMMING_TRACKER_H

#include <cstdlib>
#include "logger_msgs.h"

/*
 * Questo Tracker e' in grado di tracciare in modo distinto classi arbitrarie di qualsiasi
 * tipo. Questo e' possibile perche' una classe template che usa il pattern CRTP genera
 * un tipo differente per ogni specializzazione e quindi si hanno diversi attributi "count"
 * static, uno per ogni nuovo tipo generato, e non solo uno condiviso fra tutti.
 */

template <typename D>
class Tracker {
private:

    static u_int count;

public:
    Tracker() {
        if(debug) LOG2("Constructor for", (void*) this);
        count++;
    }

    ~Tracker() {
        if(debug) LOG2("Deconstructor for", (void*) this);
        count--;
    }

    static u_int refs() { return count; }
};


#endif //SYSTEM_PROGRAMMING_TRACKER_H
