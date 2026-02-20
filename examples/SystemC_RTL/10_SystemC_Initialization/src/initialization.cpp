#include "../include/initialization.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation

INITIALIZATION::INITIALIZATION(sc_module_name name) : sc_module(name) {
    SC_THREAD(trigger);     // No static sensitivity
    SC_THREAD(catcher_1);    // No static sensitivity
    SC_THREAD(catcher_2);    // No static sensitivity
    SC_THREAD(catcher_3);    // Has static sensitivity + dont_initialize
    
    // Static sensitivity for catcher_3 only
    sensitive << e;
    
    // Prevent catcher_3 from executing during initialization
    dont_initialize();
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "  - trigger: no static sensitivity" << endl;
    cout << "  - catcher_1: no static sensitivity, will run at init" << endl;
    cout << "  - catcher_2: has initial wait() to mimic SystemC 1.0" << endl;
    cout << "  - catcher_3: static sensitivity + dont_initialize()" << endl;
    cout << "==========================================" << endl;
}

// Trigger thread - notifies e every 2 seconds (at 1s, 3s, 5s, ...)
void INITIALIZATION::trigger(void) {
    cout << sc_time_stamp() << ": trigger thread started" << endl;
    
    while (true) {
        e.notify(1, SC_SEC);  // Notify after 1 second
        cout << sc_time_stamp() << ": trigger scheduled e for +1s" << endl;
        wait(2, SC_SEC);       // Wait 2 seconds between notifications
    }
}

// Catcher 1 - No initial wait, will run at initialization
void INITIALIZATION::catcher_1(void) {
    cout << sc_time_stamp() << ": catcher_1 thread started" << endl;
    
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_1 triggered" << std::endl;
        wait(e);  // Dynamic sensitivity
    }
}

// Catcher 2 - Has initial wait() to mimic SystemC 1.0 behavior
void INITIALIZATION::catcher_2(void) {
    cout << sc_time_stamp() << ": catcher_2 thread started" << endl;
    
    wait(e);  // Initial wait - prevents execution at initialization
              // This mimics SystemC 1.0 behavior
    
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_2 triggered" << std::endl;
        wait(e);  // Dynamic sensitivity
    }
}

// Catcher 3 - Uses dont_initialize() to prevent initialization execution
void INITIALIZATION::catcher_3(void) {
    cout << sc_time_stamp() << ": catcher_3 thread started (but dont_initialize prevents init execution)" << endl;
    
    while (true) {
        std::cout << sc_time_stamp() << ": catcher_3 triggered" << std::endl;
        wait(e);  // Dynamic sensitivity
    }
}