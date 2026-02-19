#ifndef COMBINED_H
#define COMBINED_H

#include <systemc.h>
#include <iostream>

SC_MODULE(COMBINED) {
    // Event declarations - 10 different events
    sc_event e1, e2, e3, e4, e5, e6, e7, e8, e9, e10;
    
    // Constructor declaration
    SC_CTOR(COMBINED);
    
private:
    // Thread process declarations
    void trigger(void);
    void catcher_0(void);
    void catcher_1(void);
    void catcher_2and3(void);
    void catcher_4or5(void);
    void catcher_timeour_or_6(void);
    void catcher_timeout_or_7or8(void);
    void catcher_timeout_or_9and10(void);
};

#endif