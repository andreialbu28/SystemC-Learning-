#include "../include/signal_example.h"
#include <iostream>
#include <string>

using namespace sc_core;
using std::cout;
using std::endl;

unsigned errors = 0;

int sc_main(int argc, char* argv[]) {


    // Configure time resolution
    sc_set_time_resolution(1, SC_PS);
    sc_set_default_time_unit(1, SC_NS);

    // Instantiate the module
    signal_example signal_example_i("signal_example_i");


    // Start simulation
    sc_start();


    cout << "\nSimulation completed at " << sc_time_stamp() << " with "<< " " << (errors ? "FAILED" : "PASSED")
         << " with " << errors << " errors" << endl;

    return errors ? 1 : 0;
}