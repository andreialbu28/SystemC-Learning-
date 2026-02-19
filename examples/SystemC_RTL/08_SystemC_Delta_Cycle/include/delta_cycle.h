#ifndef DELTA_CYCLE_H
#define DELTA_CYCLE_H

#include <systemc.h>
#include <iostream>

SC_MODULE(DELTA_CYCLE) {
    // Member variables
    int x, y;
    
    // Constructor declaration
    SC_CTOR(DELTA_CYCLE);
    
private:
    // Thread process declarations
    void add_x(void);
    void multiply_x(void);
    void add_y(void);
    void multiply_y(void);
};

#endif