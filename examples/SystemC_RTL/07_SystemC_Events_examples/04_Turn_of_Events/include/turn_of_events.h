#ifndef TURN_OF_EVENTS_H
#define TURN_OF_EVENTS_H

#include <systemc.h>
#include <iostream>

SC_MODULE(turn_of_events) {
    // Event declarations
    sc_event signal_stop, signals_off;
    sc_event stop_indicator_on, stop_indicator_off;
    
    // Constructor declaration
    SC_CTOR(turn_of_events);
    
private:
    // Thread process declarations
    void turn_knob_thread(void);
    void stop_signal_thread(void);
};

#endif