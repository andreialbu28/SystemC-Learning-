#include "turn_of_events.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation
turn_of_events::turn_of_events(sc_module_name name) : sc_module(name) {
    SC_THREAD(turn_knob_thread);
    SC_THREAD(stop_signal_thread);
    
    cout << "Constructor: Turn of Events module created" << endl;
    cout << "==========================================" << endl;
    cout << "Commands: S = STOP, F = OFF" << endl;
    cout << "==========================================" << endl;
}

// Turn knob thread - simulates driver input
void turn_of_events::turn_knob_thread(void) {
    enum directions {STOP = 'S', OFF = 'F'};
    char direction;  // selects appropriate indicator
    bool did_stop = false;  // allow other threads to get into waiting state

    // Wait one delta cycle to ensure other threads are ready
    wait(SC_ZERO_TIME);

    while (true) {
        cout << "\nSignal Command: ";
        cin >> direction;
        
        switch (direction) {
            case STOP:
                cout << "STOP command received" << endl;
                signals_off.notify();      // Turn off any existing signals
                signal_stop.notify();       // Signal stop condition
                wait(stop_indicator_on);    // Wait for stop indicator to turn on
                did_stop = true;
                cout << "Stop indicator is ON" << endl;
                break;

            case OFF:
                cout << "OFF command received" << endl;
                signals_off.notify();       // Signal to turn off indicators
                if (did_stop) {
                    wait(stop_indicator_off); // Wait for stop indicator to turn off
                }
                did_stop = false;
                cout << "All indicators OFF" << endl;
                break;
                
            default:
                cout << "Unknown command. Use S (STOP) or F (OFF)" << endl;
                break;
        }
    }
}

// Stop signal thread - simulates the turn signal mechanism
void turn_of_events::stop_signal_thread(void) {
    while (true) {
        wait(signal_stop);                  // Wait for stop signal
        cout << "STOPPING !!!!!!!!" << endl;
        
        stop_indicator_on.notify();         // Turn on stop indicator
        wait(signals_off);                   // Wait for signals off command
        cout << "Stop OFF -------" << endl;
        stop_indicator_off.notify();         // Turn off stop indicator
    }
}