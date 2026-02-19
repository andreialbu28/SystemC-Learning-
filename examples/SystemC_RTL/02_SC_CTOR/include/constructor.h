#ifndef MODULE_A_H
#define MODULE_A_H

#include <systemc.h>

SC_MODULE(MODULE_A) {
    // Constructor declaration
    SC_CTOR(MODULE_A);
    
private:
    // Process function declaration
    void func_a(void);
};

#endif