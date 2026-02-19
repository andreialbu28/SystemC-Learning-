#include "sensitivity.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Sensitivity Example ===" << endl;
    cout << "Demonstrates static vs dynamic sensitivity in SystemC" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    SENSITIVITY sensitivity("sensitivity");
    
    cout << "\nStarting simulation for 7 seconds..." << endl;
    cout << "Trigger 1: event1 every 2 seconds" << endl;
    cout << "Trigger 2: event2 every 3 seconds" << endl;
    cout << "Both catchers respond to event1 OR event2" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 7 seconds
    sc_start(7, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}