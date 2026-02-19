#include "combined.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation - register all threads
COMBINED::COMBINED(sc_module_name name) : sc_module(name) {
    SC_THREAD(trigger);
    SC_THREAD(catcher_0);
    SC_THREAD(catcher_1);
    SC_THREAD(catcher_2and3);
    SC_THREAD(catcher_4or5);
    SC_THREAD(catcher_timeour_or_6);
    SC_THREAD(catcher_timeout_or_7or8);
    SC_THREAD(catcher_timeout_or_9and10);
    
    cout << "Constructor: All 8 threads registered for " << name << endl;
    cout << "  Events e1-e10 will trigger at t=1s through t=10s" << endl;
}

// Trigger thread: schedules all 10 events at specific times
void COMBINED::trigger(void) {
    // Schedule events at different times
    e1.notify(1, SC_SEC);
    e2.notify(2, SC_SEC);
    e3.notify(3, SC_SEC);
    e4.notify(4, SC_SEC);
    e5.notify(5, SC_SEC);
    e6.notify(6, SC_SEC);
    e7.notify(7, SC_SEC);
    e8.notify(8, SC_SEC);
    e9.notify(9, SC_SEC);
    e10.notify(10, SC_SEC);
    
    cout << sc_time_stamp() << ": Trigger: All events scheduled" << endl;
    
    // Trigger thread exits after scheduling (no loop)
}

// Catcher 0: Simple timeout after 2 seconds
void COMBINED::catcher_0(void) {
    wait(2, SC_SEC);
    cout << sc_time_stamp() << ": 2 seconds timeout - function catcher 0" << endl;
}

// Catcher 1: Waits for single event e1
void COMBINED::catcher_1(void) {
    wait(e1);
    cout << sc_time_stamp() << ": catch e1 - function catcher 1" << endl;
}

// Catcher 2and3: Waits for both e2 AND e3 (logical AND)
void COMBINED::catcher_2and3(void) {
    wait(e2 & e3);  // Both events must occur
    cout << sc_time_stamp() << ": catch e2 and e3 - function catcher 2 and 3" << endl;
}

// Catcher 4or5: Waits for either e4 OR e5 (logical OR)
void COMBINED::catcher_4or5(void) {
    wait(e4 | e5);  // First event that occurs
    cout << sc_time_stamp() << ": catch e4 or e5 - function catcher 4 and 5" << endl;
}

// Catcher timeour_or_6: Waits for timeout (5s) OR e6
void COMBINED::catcher_timeour_or_6(void) {
    wait(sc_time(5, SC_SEC), e6);  // Timeout at 5s or e6 at 6s
    cout << sc_time_stamp() << ": 5 seconds timeout or catch e6" << endl;
}

// Catcher timeout_or_7or8: Waits for timeout (20s) OR (e7 OR e8)
void COMBINED::catcher_timeout_or_7or8(void) {
    wait(sc_time(20, SC_SEC), e7 | e8);  // Timeout at 20s or e7(7s) or e8(8s)
    cout << sc_time_stamp() << ": 20 seconds timeout or catch e7 or catch e8" << endl;
}

// Catcher timeout_or_9and10: Waits for timeout (20s) OR (e9 AND e10)
void COMBINED::catcher_timeout_or_9and10(void) {
    wait(sc_time(20, SC_SEC), e9 & e10);  // Timeout at 20s or both e9(9s) and e10(10s)
    cout << sc_time_stamp() << ": 20 seconds timeout or catch (e9 and e10)" << endl;
}