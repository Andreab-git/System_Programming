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

    // Costruttore privato
    my_shared() {};

    // Costruttore privato
    my_shared(my_shared<T> && other) = delete;


public:
    // Costruttore pubblico per inizializzare count a 1
    explicit my_shared(T *ptr): ref(ptr), count(new u_int(1)) {
        if(debug) LOG1("Chiamato costruttore con parametro T *ptr");
    }

    // Copy Constructor
    my_shared(const my_shared<T> &sh_ptr) {
        // Assegno il puntatore dell'oggetto sh_ptr a this->ref
        this->ref = sh_ptr.ref;
        // Assegno il valore di count dell'oggetto sh_ptr a this->count
        this->count = sh_ptr.count;
        // Incremento il valore di count di questo oggetto
        (*(this->count)) ++;
        if(debug) LOG1("Chiamato costruttore con parametro const my_shared<T> &sh_ptr");
    }

    // Overloading dell'operatore di assegnamento
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

    // Restituisce il valore di count di questa classe
    u_int use_count() {
        return *(this->count);
    }

    // move operators have little usefulness since the number of operations is even greater than copy
    my_shared<T>& operator=(my_shared<T> && other) = delete;

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
