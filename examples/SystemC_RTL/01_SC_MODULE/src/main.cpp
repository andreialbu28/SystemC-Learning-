#include "../include/module_A.h"
#include "../include/module_B.h"
#include "../include/module_C.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Instantiate all three modules with different names
    MODULE_A module_a("Module_A");
    MODULE_B module_b("Module_B");
    MODULE_C module_c("Module_C");

    // Start simulation
    sc_start();

    return 0;
}