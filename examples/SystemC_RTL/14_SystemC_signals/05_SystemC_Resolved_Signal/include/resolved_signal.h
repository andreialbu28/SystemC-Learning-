#ifndef RESOLVED_SIGNAL_H
#define RESOLVED_SIGNAL_H

#include <systemc.h>
#include <vector>

SC_MODULE(RESOLVED_SIGNAL) {
    // Resolved signal channel (supports multiple drivers)
    sc_signal_resolved rv;
    
    // Vector of possible 4-level logic values
    std::vector<sc_logic> levels;
    
    // Constructor declaration
    SC_CTOR(RESOLVED_SIGNAL);
    
private:
    // Thread process declarations
    void writer1(void);
    void writer2(void);
    void consumer(void);
};

#endif