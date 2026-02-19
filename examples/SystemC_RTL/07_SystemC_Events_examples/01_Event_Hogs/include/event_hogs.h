#ifndef EVENT_HOGS_H
#define EVENT_HOGS_H

#include <systemc.h>

SC_MODULE(event_hogs) {
    // Event declarations
    sc_event a_event, b_event;
    
    // Constructor declaration
    SC_CTOR(event_hogs);
    
private:
    // Thread process declarations
    void A_thread(void);
    void B_thread(void);
    void C_thread(void);
};

#endif