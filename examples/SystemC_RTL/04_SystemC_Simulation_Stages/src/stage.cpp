#include "stage.h"
#include <iostream>

// Constructor implementation
STAGE::STAGE(sc_module_name name) : sc_module(name) {
    std::cout << sc_time_stamp() << ": Elaboration: constructor" << std::endl;
    
    // Register thread process
    SC_THREAD(thread);
}

// Destructor implementation
STAGE::~STAGE() {
    std::cout << sc_time_stamp() << ": Cleanup: destructor" << std::endl;
}

// Elaboration callback - called after module hierarchy construction
// Can be used to instantiate additional modules based on hierarchy properties
void STAGE::before_end_of_elaboration() {
    std::cout << "before end of elaboration" << std::endl;
}

// Elaboration callback - called when elaboration is complete
// Cannot modify hierarchy at this point
void STAGE::end_of_elaboration() {
    std::cout << "end of elaboration" << std::endl;
}

// Simulation callback - called before simulation starts
// Used for final setup before simulation begins
void STAGE::start_of_simulation() {
    std::cout << "start of simulation" << std::endl;
}

// Simulation callback - called when simulation ends
// Used for cleanup and reporting
void STAGE::end_of_simulation() {
    std::cout << "end of simulation" << std::endl;
}

// Thread process implementation
void STAGE::thread() {
    std::cout << sc_time_stamp() << ": Execution.initialization" << std::endl;
    
    int i = 0;
    while(true) {
        wait(1, SC_SEC);  // Advance time by 1 second
        std::cout << sc_time_stamp() << ": Execution.simulation" << std::endl;
        std::cout << "i = " << i << std::endl;
        
        if (++i >= 2) {
            std::cout << "i = " << i << std::endl;
            sc_stop();  // Stop simulation after 2 iterations
        }
    }
}