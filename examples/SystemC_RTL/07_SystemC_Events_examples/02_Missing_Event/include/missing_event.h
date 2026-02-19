#ifndef MISSING_EVENT_H
#define MISSING_EVENT_H

#include <systemc.h>

SC_MODULE(missing_event) {
    sc_event a_event;
    SC_CTOR(missing_event);
    
private:
    void A_thread(void);
    void B_thread(void);
    void C_thread(void);
};

#endif