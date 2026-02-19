#ifndef STAGE_H
#define STAGE_H

#include <systemc.h>

SC_MODULE(STAGE) {
    // Constructor declaration
    SC_CTOR(STAGE);
    
    // Destructor declaration
    ~STAGE();
    
    // Elaboration callbacks
    void before_end_of_elaboration();
    void end_of_elaboration();
    
    // Simulation callbacks
    void start_of_simulation();
    void end_of_simulation();
    
private:
    // Process declaration
    void thread();
};

#endif