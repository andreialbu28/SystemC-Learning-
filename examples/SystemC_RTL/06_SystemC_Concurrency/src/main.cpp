#include "../include/concurrency.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Instantiate the module
    CONCURRENCY concurrency("concurrency");
    
    std::cout << "Starting concurrent simulation for 10 seconds..." << std::endl;
    std::cout << "Thread 1 runs every 2 seconds" << std::endl;
    std::cout << "Thread 2 runs every 4 seconds" << std::endl;
    std::cout << "----------------------------------------" << std::endl;
    
    // Run simulation for 10 seconds
    sc_start(10, SC_SEC);
    
    std::cout << "----------------------------------------" << std::endl;
    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;
    
    return 0;
}