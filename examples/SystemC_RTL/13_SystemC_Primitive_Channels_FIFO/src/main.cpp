#include "../include/fifo.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== FIFO Primitive Channel Example ===" << endl;
    cout << "Demonstrates sc_fifo for data communication between processes" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    FIFO fifo("fifo");
    
    cout << "\nStarting simulation for 10 seconds..." << endl;
    cout << "Three FIFO configurations demonstrate different aspects:" << endl;
    cout << "  FIFO 1: Blocking write/read (generator: 1s, consumer: 3s)" << endl;
    cout << "  FIFO 2: Non-blocking with events" << endl;
    cout << "  FIFO 3: Monitoring free/available slots" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Run simulation for 10 seconds
    sc_start(10, SC_SEC);
    
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}