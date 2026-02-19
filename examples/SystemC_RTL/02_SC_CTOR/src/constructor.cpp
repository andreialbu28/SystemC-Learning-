#include "../include/constructor.h"
#include <iostream>

// Constructor implementation
MODULE_A::MODULE_A(sc_core::sc_module_name name) : sc_module(name) {
    // Register func_a as a method process with the simulation kernel
    SC_METHOD(func_a);
    std::cout << "Constructor of " << name << " executed" << std::endl;
}

// Process function implementation
void MODULE_A::func_a(void) {
    std::cout << "Process func_a in module: " << name() << std::endl;
}