#include "../include/semaphore.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Semaphore Primitive Channel Example ===" << endl;
    cout << "Demonstrates counting semaphore for multiple resource access" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    SEMAPHORE semaphore("semaphore");
    
    cout << "\nStarting simulation for 20 seconds..." << endl;
    cout << "Three threads compete for 2 resources:" << endl;
    cout << "  - Thread 1: holds resource for 2 seconds" << endl;
    cout << "  - Thread 2: holds resource for 1 second" << endl;
    cout << "  - Thread 3: holds resource for 1 second" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 20 seconds
    sc_start(20, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}