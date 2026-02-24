// =============================================================================
// print_sink.cpp
// Implementation of PrintSink TDF module
// =============================================================================
#include "../include/print_sink.h"
#include <iostream>
#include <iomanip>

// Constructor implementation
PrintSink::PrintSink(sc_module_name name) : sca_tdf::sca_module(name), in("in"), sample_count(0) {}

void PrintSink::set_attributes() {
    // This sink will inherit the timestep from the connected source.
    // Leaving set_timestep() uncalled lets the AMS scheduler propagate
    // the timestep from the driving module — the safe default for sinks.
}

void PrintSink::processing() {
    std::cout << "[PrintSink]  t = "
              << std::setw(10) << sc_core::sc_time_stamp()
              << "  value = " << std::fixed << std::setprecision(3)
              << in.read() << "\n";
    ++sample_count;
}