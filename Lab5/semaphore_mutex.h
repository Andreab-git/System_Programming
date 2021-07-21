//
// Created by ff on 5/28/21.
//

#ifndef ESERCITAZIONE_5_SEMAPHORE_MUTEX_H
#define ESERCITAZIONE_5_SEMAPHORE_MUTEX_H

// semaphore with mutex: linux only, with pthread

class Semaphore {

    mutex m;
    mutex wq;
    int count;

public:

    Semaphore():count(0){wq.lock();};
    Semaphore(int n):count(n){wq.lock();};

    void acquire(){
        m.lock();
        count-=1;
        if(count < 0) {
            m.unlock();
            wq.lock();
        }

        m.unlock();
    }

    void release(){
        m.lock();
        count+=1;
        if(count <=0){
            wq.unlock();
        } else {
            m.unlock();
        }

    }
};

#endif //ESERCITAZIONE_5_SEMAPHORE_MUTEX_H
