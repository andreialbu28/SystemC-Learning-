#include "delta_cycle.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Delta Cycle Example ===" << endl;
    cout << "Demonstrates how multiple threads execute in the same delta cycle" << endl;
    cout << "and how variable updates are visible to other threads." << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    DELTA_CYCLE delta("delta");
    
    cout << "\nStarting simulation (single delta cycle)..." << endl;
    cout << "Note: All threads execute in the same delta cycle (time 0)" << endl;
    cout << "The order of execution is implementation-dependent!" << endl;
    cout << "------------------------------------------" << endl;
    
    // Start simulation - all threads execute in initial delta cycle
    sc_start();
    
    cout << "------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    cout << "Final values: x = " << delta.x << ", y = " << delta.y << endl;
    
    return 0;
}