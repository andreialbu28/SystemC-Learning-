#ifndef SIGNAL_EVENT_H
#define SIGNAL_EVENT_H

#include "systemc.h"
#include <iostream>

SC_MODULE(SIGNAL_EVENT) {
    // Two signal channels
    sc_signal<int> s1, s2;
    
    // Constructor declaration
    SC_CTOR(SIGNAL_EVENT);
    
private:
    // Thread process declarations
    void producer1(void);
    void producer2(void);
    void consumer(void);
};

#endif