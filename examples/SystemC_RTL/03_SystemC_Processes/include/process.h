#ifndef PROCESS_H
#define PROCESS_H

#include <systemc.h>

SC_MODULE(PROCESS) {
    // Clock declaration (not instantiated here, just declared)
    sc_clock clk;
    
    // Constructor declaration with initializer list
    SC_CTOR(PROCESS);
    
private:
    // Process declarations
    void method(void);
    void thread(void);
    void cthread(void);
};

#endif