#include "../include/module_A.h"
#include <iostream>

// Constructor implementation
MODULE_A::MODULE_A(sc_core::sc_module_name name) {
    std::cout << name << " constructor using the first approach" << std::endl;
}