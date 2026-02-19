#ifndef HELLO_WORLD_H
#define HELLO_WORLD_H

#include "systemc.h"
#include <iostream>

using namespace sc_core;
using namespace sc_dt;
using namespace std;

SC_MODULE(HelloWorld) {
    // Constructor
    SC_CTOR(HelloWorld);

    // Method process
    void hello2();
};

#endif