#include "../include/ordered_events.h"
#include <iostream>

ordered_events::ordered_events(sc_module_name name) : sc_module(name) {
    SC_THREAD(B_thread);
    SC_THREAD(A_thread);
    SC_THREAD(C_thread);
}

void ordered_events::A_thread(void) {
    while(true) {
        a_event.notify(1, SC_NS);
        std::cout << sc_time_stamp() << " A sent a_event!" << std::endl;
        wait(c_event);
        std::cout << sc_time_stamp() << " A got c_event" << std::endl;
    }
}

void ordered_events::B_thread(void) {
    while(true) {
        b_event.notify(1, SC_NS);
        std::cout << sc_time_stamp() << " B sent b_event" << std::endl;
        wait(a_event);
        std::cout << sc_time_stamp() << " B got a_event!" << std::endl;
    }
}

void ordered_events::C_thread(void) {
    while(true) {
        c_event.notify(1, SC_NS);
        std::cout << sc_time_stamp() << " C sent c_event" << std::endl;
        wait(b_event);
        std::cout << sc_time_stamp() << " C got b_event" << std::endl;
    }
}