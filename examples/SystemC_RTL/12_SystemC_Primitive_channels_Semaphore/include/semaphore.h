#ifndef SEMAPHORE_H
#define SEMAPHORE_H

#include <systemc.h>
#include <iostream>

SC_MODULE(SEMAPHORE) {
    // Semaphore primitive channel
    sc_semaphore s;
    
    // Constructor declaration
    SC_CTOR(SEMAPHORE);
    
private:
    // Thread process declarations
    void thread_1(void);
    void thread_2(void);
    void thread_3(void);
};

#endif