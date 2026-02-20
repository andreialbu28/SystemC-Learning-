#include "../include/signal_multi_writer.h"

using namespace sc_core;
using std::cout;
using std::endl;

int sc_main(int argc, char* argv[]) {
    cout << "=== Multi-Writer Signal Example ===" << endl;
    cout << "Demonstrates SC_MANY_WRITERS signal mode" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    MULTI multi("multi");
    
    cout << "\nStarting simulation for 3 seconds..." << endl;
    cout << "Writer 1: writes to s1 and s2 every second (incrementing: 10,11,12...)" << endl;
    cout << "Writer 2: writes to s2 only every second (decrementing: 15,14,13...)" << endl;
    cout << "Consumer 1: sensitive only to s1" << endl;
    cout << "Consumer 2: sensitive to both s1 and s2" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 3 seconds
    sc_start(3, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}