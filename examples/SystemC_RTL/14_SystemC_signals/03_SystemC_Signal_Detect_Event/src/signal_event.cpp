#include "../include/signal_event.h"
#include <iostream>

using namespace sc_core;
using std::cout;
using std::endl;

// Constructor implementation
SIGNAL_EVENT::SIGNAL_EVENT(sc_module_name name) {
    SC_THREAD(producer1);
    SC_THREAD(producer2);
    SC_THREAD(consumer);
    
    // Static sensitivity for consumer - sensitive to both signals
    // This is equivalent to: sensitive << s1.default_event() << s2.value_changed_event()
    sensitive << s1 << s2;
    
    // Prevent consumer from running at time 0
    dont_initialize();
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Consumer is statically sensitive to s1 and s2" << endl;
    cout << "dont_initialize() called - consumer won't run at time 0" << endl;
    cout << "==========================================" << endl;
}

// Producer 1 - Updates s1 every 1 second
void SIGNAL_EVENT::producer1(void) {
    int v = 1;
    while (true) {
        s1.write(v++);  // Write to s1
        cout << "s1 = " << s1 << "; s1.read() = " << s1.read() << endl;
        wait(1, SC_SEC);  // Wait 1 second between updates
    }
}

// Producer 2 - Updates s2 every 2 seconds
void SIGNAL_EVENT::producer2(void) {
    int v = 10;
    while (true) {
        s2 = v++;  // Write to s2 (using operator=)
        cout << "s2 = " << s2 << "; s2.read() = " << s2.read() << endl;
        wait(2, SC_SEC);  // Wait 2 seconds between updates
    }
}

// Consumer - Responds to signal events and identifies which signal triggered
void SIGNAL_EVENT::consumer(void) {
    while (true) {
        wait();  // Wait for any signal in sensitivity list
        
        // Check which signal(s) triggered using event() method
        if (s1.event() == true && s2.event() == true) {
            // Both signals triggered in same delta cycle
            cout << "@ time = " << sc_time_stamp() << ": s1 & s2 triggered" << endl;
        } else if (s1.event() == true) {
            // Only s1 triggered
            cout << "@ time = " << sc_time_stamp() << ": s1 triggered" << endl;
        } else {
            // Only s2 triggered (or possibly neither, but wait() returned due to sensitivity)
            cout << "@ time = " << sc_time_stamp() << ": s2 triggered" << endl;
        }
    }
}