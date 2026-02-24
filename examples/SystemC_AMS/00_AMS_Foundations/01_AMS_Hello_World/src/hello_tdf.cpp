// =============================================================================
// hello_tdf.cpp
// Implementation of HelloTDF TDF module
// =============================================================================
#include "../include/hello_tdf.h"
#include <iostream>

// Constructor implementation
HelloTDF::HelloTDF(sc_module_name name) : sca_tdf::sca_module(name), out("out"), sample_count(0) {}

// -------------------------------------------------------------------------
// set_attributes() implementation
// -------------------------------------------------------------------------
void HelloTDF::set_attributes() {
    // Execute processing() every 1 microsecond → 1 MHz "sample rate"
    set_timestep(sca_core::sca_time(1.0, SC_US));
}

// -------------------------------------------------------------------------
// processing() implementation
// -------------------------------------------------------------------------
void HelloTDF::processing() {
    // Produce a simple counting ramp: 0.0, 0.1, 0.2, ...
    double value = sample_count * 0.1;
    out.write(value);

    // Print the first 5 samples so we can see the module is running
    if (sample_count < 5) {
        std::cout << "[HelloTDF] t = " << sc_core::sc_time_stamp()
                  << "  sample[" << sample_count << "] = " << value << "\n";
    }
    ++sample_count;
}