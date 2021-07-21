//
// Created by ff on 5/28/21.
//

#ifndef ESERCITAZIONE_5_SEMAPHORE_H
#define ESERCITAZIONE_5_SEMAPHORE_H

#include <iostream>
#include <condition_variable>
#include <mutex>

class CountingSemaphore {
    int count  = 0;
    std::condition_variable cv;
    std::mutex m;

public:

    CountingSemaphore(int n):count(n){};

    void acquire() {
        std::unique_lock<std::mutex> lk(m);
        cv.wait(lk, [this](){return count>0;});
        count--;
    }

    void release() {
        //std::cout<<"+"<<count<<std::endl;
        std::unique_lock<std::mutex> lk(m);
        //std::cout<<"-"<<count<<std::endl;
        count++;
        lk.unlock(); // just to avoid to wake a thread and make it sleep immediately
        cv.notify_one();
    }
};

#endif //ESERCITAZIONE_5_SEMAPHORE_H
