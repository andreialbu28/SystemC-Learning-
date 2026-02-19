#include "../include/combined.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== COMBINED Events Example ===" << endl;
    cout << "Demonstrates various event waiting patterns:" << endl;
    cout << "  - Single event waits" << endl;
    cout << "  - AND combinations (both events)" << endl;
    cout << "  - OR combinations (first event)" << endl;
    cout << "  - Timeout vs event combinations" << endl;
    cout << "  - Complex combinations (timeout OR (AND))" << endl;
    cout << "================================\n" << endl;
    
    // Instantiate the module
    COMBINED combined("combined");
    
    cout << "\nStarting simulation (will run until all events processed)..." << endl;
    cout << "Event timing: e1=1s, e2=2s, e3=3s, e4=4s, e5=5s," << endl;
    cout << "              e6=6s, e7=7s, e8=8s, e9=9s, e10=10s" << endl;
    cout << "--------------------------------------------------------\n" << endl;
    
    // Start simulation (runs until no more events)
    sc_start();
    
    cout << "\n--------------------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}