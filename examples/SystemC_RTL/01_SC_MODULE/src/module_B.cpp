#include "../include/module_B.h"
#include <iostream>

// Constructor implementation
MODULE_B::MODULE_B(sc_core::sc_module_name name) {
    std::cout << name << " constructor using the second approach" << std::endl;
}