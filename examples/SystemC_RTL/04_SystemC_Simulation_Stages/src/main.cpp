#include "../include/stage.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Elaboration phase: Module construction
    STAGE stage("stage");
    
    // Simulation phase: Run until sc_stop() is called
    sc_start();
    
    // Cleanup phase: Destructor called automatically
    return 0;
}