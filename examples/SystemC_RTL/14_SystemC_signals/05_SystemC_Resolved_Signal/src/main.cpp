#include "../include/resolved_signal.h"

using namespace sc_core;
using namespace sc_dt;
using std::cout;
using std::endl;

int sc_main(int argc, char* argv[]) {
    cout << "=== Resolved Signal Example ===" << endl;
    cout << "Demonstrates sc_signal_resolved with multiple drivers" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    RESOLVED_SIGNAL resolved("resolved");
    
    cout << "\nStarting simulation for 17 seconds..." << endl;
    cout << "This will test all 16 resolution combinations" << endl;
    cout << "Resolution table will be printed as output" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 17 seconds (tests all combinations)
    sc_start(17, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}