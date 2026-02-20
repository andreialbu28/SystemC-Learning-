#ifndef SIGNAL_EXAMPLE_H
#define SIGNAL_EXAMPLE_H

#include "systemc.h"
#include <iostream>
#include <string>

SC_MODULE(signal_example) {
    // Enumeration for traffic light colors
    enum color { BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE };

    // Local Data Variables
    int count;
    color traffic_temp;
    std::string message_temp;

    // Local channels
    sc_signal<int> count_sig;
    sc_signal<color> traffic_sig;
    sc_signal<std::string> message_signal;

    // Constructor declaration
    SC_CTOR(signal_example);

private:
    // Process declarations
    void signal_example_thread();
    void signal_example_method();
};

#endif