#include "systemc.h"

using namespace sc_core;


SC_MODULE ( MODULE_A)
{
    SC_CTOR(MODULE_A)
    {
        SC_METHOD(func_a);

    }

    void func_a(void)
    {
        std::cout<<name()<<std::endl ;
    }
};



int sc_main ( int argc, char* argv[])
{
    MODULE_A Module_a("Module_a");
    return 0; 
}