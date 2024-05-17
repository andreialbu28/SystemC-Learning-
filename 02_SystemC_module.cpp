#include "systemc.h"

using namespace sc_core;

SC_MODULE (MODULE_A)
{
    SC_CTOR(MODULE_A)
    {
        std::cout << name() << " constructor using the first approach" << std::endl ;
    }
};

struct MODULE_B : public sc_module
{
    SC_CTOR(MODULE_B)
    {
        std::cout << name() << " constructor using the second approach" << std::endl ;
    }
};

class MODULE_C : public sc_module 
{
    public : 
    SC_CTOR(MODULE_C)
    {
        std::cout << name() << " constructor using the third approach" << std::endl ;
    }
};


int sc_main ( int argc, char* argv[])
{
    MODULE_A module_a("Module_A");

    MODULE_B module_b("Module_B");

    MODULE_C module_c("Module_C");

    sc_start() ;

    return 0; 
}