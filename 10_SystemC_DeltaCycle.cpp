#include "systemc.h"
using namespace sc_core ; 

SC_MODULE(DELTA_CYCLE)
{
    int x= 1, y=1 ; 
    SC_CTOR(DELTA_CYCLE)
    {
        SC_THREAD(add_x) ;
        SC_THREAD(multiply_x) ;
        SC_THREAD(add_y); 
        SC_THREAD(multiply_y);
    }

    void add_x(void)
    {
        std::cout << "ADD X: " << x << "+ 2 " << "= " ;
        x+=2 ;
        std::cout << x << std::endl; 
    }

    void multiply_x(void)
    {
        std::cout << "MULTIPLY X: " << x << "* 3 " << "= " ;
        x*=2 ;
        std::cout << x << std::endl; 
    } 

    
    void add_y(void)
    {
        std::cout << "ADD Y: " << y << "+ 2 " << "= " ;
        y+=2 ;
        std::cout << y << std::endl; 
    }

    void multiply_y(void)
    {
        std::cout << "MULTIPLY Y: " << y << "* 3 " << "= " ;
        y*=2 ;
        std::cout << y << std::endl; 
    } 
};

int sc_main(int argc, char*[])
{
    DELTA_CYCLE delta("delta") ; 
    sc_start(); 
    return 0 ;
}