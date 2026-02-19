#include "event_hogs.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    std::cout << "=== Event Hogs Example: Immediate Notifications ===" << std::endl;
    std::cout << "This demonstrates how immediate notify() affects process scheduling" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    // Instantiate the module
    event_hogs bad_use("bad_use");
    
    std::cout << "\nStarting simulation..." << std::endl;
    std::cout << "Thread A and B are synchronized via immediate events" << std::endl;
    std::cout << "Thread C runs independently every 1 ns" << std::endl;
    std::cout << "Thread B will stop simulation after 10 iterations" << std::endl;
    std::cout << "==================================================\n" << std::endl;
    
    // Start simulation (runs until sc_stop() is called)
    sc_start();
    
    std::cout << "\n==================================================" << std::endl;
    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;
    
    return 0;
}