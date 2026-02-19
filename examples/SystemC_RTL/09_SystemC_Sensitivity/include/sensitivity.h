#ifndef SENSITIVITY_H
#define SENSITIVITY_H

#include <systemc.h>
#include <iostream>

SC_MODULE(SENSITIVITY) {
    // Event declarations
    sc_event event1, event2;
    
    // Constructor declaration
    SC_CTOR(SENSITIVITY);
    
private:
    // Thread process declarations
    void trigger_1(void);
    void trigger_2(void);
    void catch_1or2_dyn(void);
    void catch_1or2_static(void);
};

#endif