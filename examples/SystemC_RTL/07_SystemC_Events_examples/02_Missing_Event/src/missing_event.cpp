#include "../include/missing_event.h"

missing_event::missing_event(sc_module_name name) : sc_module(name) {
    SC_THREAD(B_thread);
    SC_THREAD(A_thread);
    SC_THREAD(C_thread);
}

void missing_event::A_thread(void) {
    a_event.notify(SC_ZERO_TIME);  // DELTA notification - SAFE!
    cout << "A scheduled a_event (delta)!" << endl;
}

void missing_event::B_thread(void) {
    wait(a_event);
    cout << "B got a_event!" << endl;
}

void missing_event::C_thread(void) {
    wait(a_event);
    cout << "C got a_event!" << endl;
}