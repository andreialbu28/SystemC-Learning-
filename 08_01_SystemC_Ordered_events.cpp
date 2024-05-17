#include "systemc.h"
using namespace sc_core ; 


SC_MODULE (ordered_events)
{
    sc_event a_event, b_event, c_event ;
    SC_CTOR(ordered_events)
    {
        SC_THREAD ( B_thread) ; 
        SC_THREAD ( A_thread) ; 
        SC_THREAD ( C_thread) ;
    }

    void A_thread (void )
    {
        while(true)
        {
            a_event.notify(SC_ZERO_TIME) ; 
            std::cout << "A sent a_event!" << std::endl ;
            wait(c_event);
            std::cout << " A got c_event" << std::endl ;
        }
    }

     void B_thread (void )
    {
        while(true)
        {
            b_event.notify(SC_ZERO_TIME) ;
            std::cout << "B sent b_event"<<std::endl ;
            wait(a_event) ;
            std::cout << "B got a_event!" << std::endl ;
        }
    }

     void C_thread (void )
    {
        while (true)
        {
            c_event.notify(SC_ZERO_TIME) ; 
            std::cout << "C sent c_event"<< std::endl;
            wait(b_event) ; 
            std::cout << "C got b_event" << std::endl ;
        }
    }
};

int sc_main ( int argc, char* argv[])
{
    ordered_events order_ev("order_ev");
    sc_start (3,SC_NS);
    return 0 ;
}