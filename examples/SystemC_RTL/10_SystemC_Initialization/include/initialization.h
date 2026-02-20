#ifndef INITIALIZATION_H
#define INITIALIZATION_H

#include <systemc.h>
#include <iostream>

SC_MODULE(INITIALIZATION) {
    // Event declaration
    sc_event e;
    
    // Constructor declaration
    SC_CTOR(INITIALIZATION);
    
private:
    // Thread process declarations
    void trigger(void);
    void catcher_1(void);
    void catcher_2(void);
    void catcher_3(void);
};

#endif