#ifndef CONCURRENCY_H
#define CONCURRENCY_H

#include <systemc.h>

SC_MODULE(CONCURRENCY) {
    // Constructor declaration
    SC_CTOR(CONCURRENCY);
    
private:
    // Thread process declarations
    void thread_1(void);
    void thread_2(void);
};

#endif