#include "../include/process.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Instantiate the PROCESS module
    PROCESS process("process");
    
    std::cout << "Execution phase begins @ " << sc_time_stamp() << std::endl;
    
    // Run simulation for 5 seconds
    sc_start(5, SC_SEC);
    
    std::cout << "Execution phase ends @ " << sc_time_stamp() << std::endl;
    
    return 0;
}