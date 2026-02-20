#ifndef MUTEX_H
#define MUTEX_H

#include "systemc.h"
#include <iostream>

SC_MODULE(MUTEX) {
    // Mutex primitive channel
    sc_mutex m;
    
    // Constructor declaration
    SC_CTOR(MUTEX);
    
private:
    // Thread process declarations
    void thread_1(void);
    void thread_2(void);
    void thread_3(void);
};

#endif