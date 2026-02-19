#include "systemc.h"
using namespace sc_core ; 


SC_MODULE (missing_event)
{
    sc_event a_event ;
    SC_CTOR(missing_event)
    {
        SC_THREAD ( B_thread) ; // ordered to cause
        SC_THREAD ( A_thread) ; // problems
        SC_THREAD ( C_thread) ;
    }

    void A_thread (void )
    {
        a_event.notify() ; // immediate
        cout << "A sent a_event!" << endl ;
    }

     void B_thread (void )
    {
        wait(a_event) ;
        cout << "B got a_event!" << endl ;
    }

     void C_thread (void )
    {
        wait(a_event) ;
        cout << "C got a_event!" << endl ;
    }
};

int sc_main ( int argc, char* argv[])
{
    missing_event miss_event("miss_event");
    sc_start (20,SC_SEC);
    return 0 ;
}


/*
    Consider what it means to execute wait(SC_ZERO_TIME). What does it mean to advance time by zero?
    
    The primary effect is that a process waiting for zero-time will resume after all the runnable processes have yielded. Since zero is always closer than any 
    other time, then all processes waiting for zero-time will be next in line to become runnable.

    This feature can be very useful. Recall the rule that to observe an event, the observer must be watching for the event prior to its notification.
    
    Now imagine we have three processes, A_thread, B_thread, and C_thread. Suppose they execute in the order A_thread, B_thread, C_thread.
    --> Furthermore, notice that A_thread does an immediate notification of an event, a_event, which B_thread and C_thread are going to wait for.

    If either B_thread or C_thread have not issued the wait(a_event) call prior to A_thread notifying the event, then they will miss the event. If the event
        never happens again, then when B_thread or C_thread issue the wait(a_event) call, they will wait forever.
    If the event happened a second time, then B_thread or C_thread would continue, but they would have missed one of the events. Missing an event can be
        devastating to a simulation. 
        
    This situation can be avoided by use of the zero-time delayed notification, notify(SC_ZERO_TIME). The reason is that delayed
        notifications are issued only after completing all runnable processes.
    Lest you think that you can simply fix the problem by ordering the processes appropriately, recall that SystemC implementations are free to choose processes from
        

*/