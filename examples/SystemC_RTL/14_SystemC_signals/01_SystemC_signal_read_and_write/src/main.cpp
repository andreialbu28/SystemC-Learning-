#include "../include/signal.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Signal Primitive Channel Example ===" << endl;
    cout << "Demonstrates sc_signal behavior with delta cycles" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    SIGNAL signal("signal");
    
    // Write to signal from outside the module (in sc_main)
    cout << "Setting signal from sc_main before simulation:" << endl;
    signal.s = -1;  // Direct assignment in sc_main
    cout << "signal.s = -1 (direct assignment in sc_main)" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Start simulation
    sc_start();
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}