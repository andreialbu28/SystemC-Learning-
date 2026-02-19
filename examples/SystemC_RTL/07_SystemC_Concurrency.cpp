/*
    Many activities in a real system occur at the same time or concurrently. For example, when simulating something like a traffic pattern with multiple cars, the goal
        is to model the vehicles independently. In other words, the cars operate in parallel.

    Software typically executes using a single thread of activity because there is usually only one processor on which to run, and partly because a thread is much
        easier to manage.
        
    On the other hand, in real systems many things occur simultaneously. For example, when an automobile executes a left turn, it is likely that the left turn indicator
        is flashing, the brakes are engaged to slow down the vehicle, engine power is decreased as the driver lets off the accelerator, and the transmission is
        shifting to a lower gear. All of these activities can occur at the same instant.

    SystemC uses simulation processes to model concurrency. As with most event-driven simulators, concurrency is not true concurrent execution. In fact, simulated
        concurrency works like cooperative multitasking. When a simulation process runs, it is expected to execute a small segment of
        code and then return control to the simulation kernel. The SystemC simulator depends on a << cooperative multitasking (non-preemptive) kernel >> 
        that cannot force a running process to return control. This feature is unlike many operating systems that preemptively interrupt running processes to
        switch to a different process.

    A POORLY BEHAVED PROCESS THAT HOGS CONTROL AND DOESN'T YIELD CONTROL TO THE SIMULATION KERNEL WILL CAUSE SYSTEMC SIMULATION TO HANG.
*/

/*
    SystemC simulation processes are simply C++ functions designated by the programmer to be used as processes. You simply tell the simulation kernel which
        functions are to be used as simulation processes. This action is known as process registration. The simulation kernel then schedules and calls each of
        these functions as needed.

    We can register one type of process  using the SC_THREAD. Its syntax was simple as shown below:

                                                                   SC_THREAD (MEMBER_FUNCTION);

    SC_THREAD ha a few restrictions: 
        1. It can be used only within a SystemC module; hence, the function must be a member function of the module class.
        2. It must be used only during the elaboration stage.  
*/

#include "systemc.h"

using namespace sc_core ; 


SC_MODULE ( CONCURRENCY )
{
    SC_CTOR ( CONCURRENCY )
    {
        SC_THREAD ( thread_1 ) ; 
        SC_THREAD ( thread_2 ) ;
    }  


    void thread_1 (void)
    {
        while(true)
        {
            std::cout << sc_time_stamp() << ": thread 1 " << std::endl ; 

            wait(2,SC_SEC) ; 
        }
    }

      void thread_2 (void)
    {
        while(true)
        {
            std::cout << "     " << sc_time_stamp() << ": thread 2 " << std::endl ; 

            wait(4,SC_SEC) ; 
        }
    }
};

int sc_main ( int argc, char* argv[])
{
    CONCURRENCY concurrency("concurrency") ; 
    sc_start ( 10, SC_SEC ) ; 

    return 0 ; 
}

