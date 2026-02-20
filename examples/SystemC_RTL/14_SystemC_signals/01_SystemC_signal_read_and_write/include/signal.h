#ifndef SIGNAL_H
#define SIGNAL_H

#include "systemc.h"
#include <iostream>

SC_MODULE(SIGNAL) {
    // Signal declaration
    sc_signal<int> s;
    
    // Constructor declaration
    SC_CTOR(SIGNAL);
    
private:
    // Thread process declaration
    void readwrite(void);
};

#endif