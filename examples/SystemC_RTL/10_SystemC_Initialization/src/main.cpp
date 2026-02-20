#include "../include/initialization.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Initialization Phase Example ===" << endl;
    cout << "Demonstrates SystemC 2.0 initialization behavior" << endl;
    cout << "and the use of dont_initialize()" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    INITIALIZATION init("init");
    
    cout << "\nStarting simulation for 4 seconds..." << endl;
    cout << "Note the initialization behavior at time 0:" << endl;
    cout << "  - catcher_1: runs at init (no dont_initialize)" << endl;
    cout << "  - catcher_2: has initial wait(), so doesn't run at init" << endl;
    cout << "  - catcher_3: dont_initialize(), so doesn't run at init" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 4 seconds
    sc_start(4, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}