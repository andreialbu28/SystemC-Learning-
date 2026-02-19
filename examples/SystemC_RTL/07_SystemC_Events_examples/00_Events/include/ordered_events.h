#ifndef ORDERED_EVENTS_H
#define ORDERED_EVENTS_H

#include <systemc.h>

SC_MODULE(ordered_events) {
    // Event declarations
    sc_event a_event, b_event, c_event;
    
    // Constructor declaration
    SC_CTOR(ordered_events);
    
private:
    // Thread process declarations
    void A_thread(void);
    void B_thread(void);
    void C_thread(void);
};

#endif