#include "../include/constructor.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Instantiate the module
    MODULE_A Module_a("Module_a");
    
    // Start the simulation - this will trigger the registered SC_METHOD
    std::cout << "Starting simulation..." << std::endl;
    sc_start();
    std::cout << "Simulation finished." << std::endl;
    
    return 0;
}