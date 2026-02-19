#ifndef MODULE_B_H
#define MODULE_B_H

#include "systemc.h"

using namespace sc_core; 

// Using struct inheritance (Approach 2)
struct MODULE_B : public sc_module {
    SC_CTOR(MODULE_B);  // Constructor declaration
};

#endif