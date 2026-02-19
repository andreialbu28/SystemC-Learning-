#ifndef MODULE_C_H
#define MODULE_C_H

#include "systemc.h"

// Using class inheritance (Approach 3)
class MODULE_C : public sc_module {
public:
    SC_CTOR(MODULE_C);  // Constructor declaration
};

#endif