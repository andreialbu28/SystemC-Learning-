#include "../include/module_C.h"
#include <iostream>

// Constructor implementation
MODULE_C::MODULE_C(sc_core::sc_module_name name) {
    std::cout << name << " constructor using the third approach" << std::endl;
}