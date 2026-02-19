#include "../include/time_utils.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    
    // Step 1: Configure the time system
    configure_time_system();
    
    // Step 2: Print time unit conversions
    print_time_conversions();
    
    // Step 3: Run simulation and display formatted time
    run_simulation_and_display_time();
    
    return 0;
}