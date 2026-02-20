#include "../include/signal_event.h"

using namespace sc_core;
using std::cout;
using std::endl;

int sc_main(int argc, char* argv[]) {
    cout << "=== Signal Event Example ===" << endl;
    cout << "Demonstrates signal events and event() method" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    SIGNAL_EVENT signal_event("signal_event");
    
    cout << "\nStarting simulation for 8 seconds..." << endl;
    cout << "Producer1: updates s1 every 1 second" << endl;
    cout << "Producer2: updates s2 every 2 seconds" << endl;
    cout << "Consumer: triggered by any signal change" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 8 seconds
    sc_start(8, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}