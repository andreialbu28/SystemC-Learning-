#ifndef MODULE_A_H
#define MODULE_A_H

#include "systemc.h"

using namespace sc_core; 

// Using SC_MODULE macro (Approach 1)
SC_MODULE(MODULE_A) 
{
    SC_CTOR(MODULE_A);  // Constructor declaration
};

#endif