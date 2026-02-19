#include "systemc.h"
using namespace sc_core ;

SC_MODULE ( event_hogs )
{
    sc_event a_event, b_event ;
    SC_CTOR(event_hogs)
    {
        SC_THREAD(A_thread);
        SC_THREAD(B_thread);
        SC_THREAD(C_thread);

        sc_set_stop_mode (SC_STOP_IMMEDIATE) ; // ensures that sc_stop() works
    }

    void A_thread(void)
    {
        while(true)
        {
            std::cout << "A @ " << sc_time_stamp() << std::endl ;
            a_event.notify() ; // immediate
            wait(b_event) ;
        }
    }

    void B_thread(void)
    {
        int count(10) ; // limit execution
        while(true)
        {
            std::cout << "B @ " << sc_time_stamp() << std::endl ;
            b_event.notify() ; // immediate
            wait(a_event) ;
            if (count-- == 0) sc_stop() ;
        }
    }

    void C_thread(void)
    {
        while(true)
        {
            std::cout << "C @ " << sc_time_stamp() << std::endl ;
            wait(1,SC_NS) ;
        }
    }
}; 



int sc_main(int argc, char* argv[])
{
    event_hogs bad_use("bad_use");
    std::cout << " 08_01_Ordered_events" << std::endl ;

    sc_start();
    return 0 ; 
}

/*
    WHEN A PROCESS EXECUTES NOTIFY(VOID) IT MAY CAUSE ONE OR MORE PROCESSES IN THE WAITING SET TO BE MOVED INTO THE RUNNABLE SET
*/