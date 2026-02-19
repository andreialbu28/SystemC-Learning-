#include "event_hogs.h"
#include <iostream>

using namespace sc_core;

// Constructor implementation
event_hogs::event_hogs(sc_module_name name) : sc_module(name) {
    SC_THREAD(A_thread);
    SC_THREAD(B_thread);
    SC_THREAD(C_thread);
    
    // Set stop mode to immediate for responsive simulation end
    sc_set_stop_mode(SC_STOP_IMMEDIATE);
    
    std::cout << "Constructor: All threads registered for " << name << std::endl;
}

// Thread A: Notifies a_event (immediate), waits for b_event
void event_hogs::A_thread(void) {
    while(true) {
        std::cout << "A @ " << sc_time_stamp() << std::endl;
        a_event.notify();  // Immediate notification (same delta cycle)
        wait(b_event);      // Wait for B's notification
    }
}

// Thread B: Notifies b_event (immediate), waits for a_event, counts iterations
void event_hogs::B_thread(void) {
    int count = 10;  // Limit execution to 10 iterations
    
    while(true) {
        std::cout << "B @ " << sc_time_stamp() << std::endl;
        b_event.notify();  // Immediate notification (same delta cycle)
        wait(a_event);      // Wait for A's notification
        
        if (count-- == 0) {
            std::cout << "B: Count reached zero, stopping simulation at " 
                      << sc_time_stamp() << std::endl;
            sc_stop();  // Stop simulation
        }
    }
}

// Thread C: Independent thread running every 1 ns
void event_hogs::C_thread(void) {
    while(true) {
        std::cout << "C @ " << sc_time_stamp() << std::endl;
        wait(1, SC_NS);  // Wait for 1 nanosecond
    }
}