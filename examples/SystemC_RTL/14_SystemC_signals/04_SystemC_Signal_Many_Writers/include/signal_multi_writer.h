#ifndef MULTI_WRITER_H
#define MULTI_WRITER_H

#include "systemc.h"
#include <iostream>

SC_MODULE(MULTI) {
    // Single-writer signal (default)
    sc_signal<int> s1;
    
    // Multi-writer signal - explicitly allows multiple writers
    sc_signal<int, SC_MANY_WRITERS> s2;
    
    // Constructor declaration
    SC_CTOR(MULTI);
    
private:
    // Thread process declarations
    void writer_1(void);
    void writer_2(void);
    void consumer_1(void);
    void consumer_2(void);
};

#endif