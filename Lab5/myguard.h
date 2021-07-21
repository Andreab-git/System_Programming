//
// Created by ff on 5/30/21.
//

#ifndef ESERCITAZIONE_5_MYGUARD_H
#define ESERCITAZIONE_5_MYGUARD_H

#include <mutex>

class MyGuard {

    std::mutex *_m;
    MyGuard(const MyGuard &m) = delete;
    MyGuard &operator = (const MyGuard &m) = delete;
public:

    MyGuard(std::mutex &m):_m(&m){
        _m->lock();
    };

    // a lock guard could capture another lock, first releasing its own lock
    MyGuard& operator =(MyGuard &&other) {
        if(this != &other) {
            _m->unlock();
            _m = other._m;
            other._m = nullptr;
        }
        return *this;
    }

    ~MyGuard() {
        if(_m!= nullptr)
            _m->unlock();
    }

};


#endif //ESERCITAZIONE_5_MYGUARD_H
