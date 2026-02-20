#include "../include/mutex.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Mutex Primitive Channel Example ===" << endl;
    cout << "Demonstrates mutual exclusion for resource access" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    MUTEX mutex("mutex");
    
    cout << "\nStarting simulation for 25 seconds..." << endl;
    cout << "Three threads compete for a shared resource:" << endl;
    cout << "  - Thread 1: holds resource for 1 second" << endl;
    cout << "  - Thread 2: holds resource for 3 seconds" << endl;
    cout << "  - Thread 3: holds resource for 5 seconds" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 25 seconds
    sc_start(25, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}