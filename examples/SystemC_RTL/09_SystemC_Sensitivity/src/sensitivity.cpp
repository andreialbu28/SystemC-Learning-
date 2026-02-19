#include "sensitivity.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation
SENSITIVITY::SENSITIVITY(sc_module_name name) : sc_module(name) {
    SC_THREAD(trigger_1);
    SC_THREAD(trigger_2);
    SC_THREAD(catch_1or2_dyn);
    SC_THREAD(catch_1or2_static);
    
    // Static sensitivity: this process is sensitive to both events
    // This applies to catch_1or2_static thread
    sensitive << event1 << event2;
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Static sensitivity set: catch_1or2_static sensitive to event1 and event2" << endl;
    cout << "==========================================" << endl;
}

// Trigger 1: Notifies event1 every 2 seconds
void SENSITIVITY::trigger_1(void) {
    // Delay by one delta cycle to ensure catchers are ready
    wait(SC_ZERO_TIME);
    
    while (true) {
        event1.notify();  // Immediate notification
        cout << sc_time_stamp() << ": Trigger 1 - event1 notified" << endl;
        wait(2, SC_SEC);  // Dynamic sensitivity - wait 2 seconds
    }
}

// Trigger 2: Notifies event2 every 3 seconds
void SENSITIVITY::trigger_2(void) {
    // Delay by one delta cycle to ensure catchers are ready
    wait(SC_ZERO_TIME);
    
    while (true) {
        event2.notify();  // Immediate notification
        cout << sc_time_stamp() << ": Trigger 2 - event2 notified" << endl;
        wait(3, SC_SEC);  // Dynamic sensitivity - wait 3 seconds
    }
}

// Dynamic sensitivity catcher: explicitly waits for event1 OR event2
void SENSITIVITY::catch_1or2_dyn(void) {
    while (true) {
        // Dynamic sensitivity - explicitly wait for events
        wait(event1 | event2);  // OR combination
        cout << "Dynamic sensitivity: event1 or event2 @ " << sc_time_stamp() << endl;
    }
}

// Static sensitivity catcher: relies on static sensitivity list
void SENSITIVITY::catch_1or2_static(void) {
    while (true) {
        wait();  // Static sensitivity - uses the sensitive list from constructor
        cout << "Static sensitivity: event1 or event2 @ " << sc_time_stamp() << endl;
    }
}