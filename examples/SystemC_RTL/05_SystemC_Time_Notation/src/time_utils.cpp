#include "../include/time_utils.h"
#include <iostream>

using namespace sc_core;

void configure_time_system() {
    // Suppress deprecation warning for set_time_resolution
    sc_report_handler::set_actions("/IEEE_Std_1666/deprecated", SC_DO_NOTHING);
    
    // Set time resolution to femtoseconds (highest precision)
    sc_set_time_resolution(1, SC_FS);
    
    // Set default time unit to seconds
    sc_set_default_time_unit(1, SC_SEC);
    
    std::cout << "=== Time System Configured ===" << std::endl;
    std::cout << "Resolution: 1 femtosecond (1e-15 s)" << std::endl;
    std::cout << "Default unit: 1 second" << std::endl;
    std::cout << std::endl;
}

void print_time_conversions() {
    std::cout << "=== Time Unit Conversions ===" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_SEC).to_default_time_units() << " seconds (default unit)" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_MS).to_default_time_units() << " seconds (1e-3)" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_US).to_default_time_units() << " seconds (1e-6)" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_NS).to_default_time_units() << " seconds (1e-9)" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_PS).to_default_time_units() << " seconds (1e-12)" << std::endl;
    std::cout << "1 second = " << sc_time(1, SC_FS).to_default_time_units() << " seconds (1e-15)" << std::endl;
    std::cout << std::endl;
}

void run_simulation_and_display_time() {
    // Run simulation for 10000 seconds
    std::cout << "=== Running Simulation for 10000 seconds ===" << std::endl;
    sc_start(10000, SC_SEC);
    
    // Get current simulation time in seconds
    double t = sc_time_stamp().to_seconds();
    
    // Convert to hours, minutes, seconds
    int hours = static_cast<int>(t) / 3600;
    int minutes = (static_cast<int>(t) % 3600) / 60;
    int seconds = static_cast<int>(t) % 60;
    
    std::cout << "Simulation time: " << t << " seconds" << std::endl;
    std::cout << "Formatted: " << hours << " hours, " << minutes << " minutes, " << seconds << " seconds" << std::endl;
}