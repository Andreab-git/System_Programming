//
// Created by ff on 5/14/21.
//

#ifndef SYSTEM_PROGRAMMING_MYSHARED_H
#define SYSTEM_PROGRAMMING_MYSHARED_H

#include <algorithm>

template <typename T>
class my_shared {

    T *ref = nullptr;
    u_int *count = nullptr;

    my_shared() {};

    // move operators have little usefulness since the number of operations is even greater than copy
    my_shared(my_shared<T> && other) = delete;
    my_shared<T>& operator=(my_shared<T> && other) = delete;

public:
    explicit my_shared(T *p):ref(p), count(new u_int(1)) { }

    my_shared(const my_shared<T> &sp) {
        this->ref = sp.ref;
        this->count = sp.count;
        (*(this->count)) ++;
    }

    my_shared<T> &operator=(my_shared<T> other) {
        swap(*this, other);
        return *this;
    }

    T* operator->() {
        return this->ref;
    }

    T& operator*() {
        return *ref;
    }

    u_int use_count() {
        return *(this->count);
    }

    ~my_shared(){
        (*(this->count))--;
        if(*(this->count) == 0){
            delete ref;
            delete  count;
        }
    }

    template <typename C> friend void swap(my_shared<C> &a, my_shared<C> &b);
};

template <typename T>
void swap(my_shared<T> &a, my_shared<T> &b){
    std::swap(a.count, b.count);
    std::swap(a.ref, b.ref);
};


#endif //SYSTEM_PROGRAMMING_MYSHARED_H
