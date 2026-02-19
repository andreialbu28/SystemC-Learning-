/*
The sensitivity of a process instance is the set of events and time-outs that can potentially cause the process to be resumed or triggered.
A process instance is said to be sensitive to an event if the event has been added to the static sensitivity or dynamic sensitivity of the process instance.
A time-out occurs when a given time interval has elapsed.

Two types of sensitivities:
  1. Static sensitivity is fixed during elaboration, supported with a sensitivity list for each process in a module.
  2. Dynamic sensitivity may vary over time under the control of the process itself, support with wait() for a thread, or next_trigger() for a method.
  
*/


#include "systemc.h"
using namespace sc_core ; 

SC_MODULE(SENSITIVITY)
{
    sc_event event1,event2 ; 
    
    SC_CTOR(SENSITIVITY)
    {
        SC_THREAD(trigger_1);
        SC_THREAD(trigger_2);
        SC_THREAD(catch_1or2_dyn) ;
        SC_THREAD(catch_1or2_static);

        sensitive << event1 << event2 ;
    }

    void trigger_1(void)
    {
        wait(SC_ZERO_TIME); // delay trigger by a delta cycle, make sure catcher is ready
        while (true) 
        {
            event1.notify(); 
            wait(2, SC_SEC); // dynamic sensitivity, re-trigger after 2 s
        }
    }

    void trigger_2(void) // delay trigger by a delta cycle
    { 
        wait(SC_ZERO_TIME);
        while (true)
         {
            event2.notify();
            wait(3, SC_SEC); // dynamic sensitivity, re-trigger after 3 s
        }
    }

    void catch_1or2_dyn(void) 
    {
        while (true) 
        {
        wait(event1 | event2); // dynamic sensitivity
        std::cout << "Dynamic sensitivty: event1 or event2 @ " << sc_time_stamp() << std::endl;
        }
  }
  void catch_1or2_static(void) { 
    while (true) 
    {
      wait(); // static sensitivity
      std::cout << "Static sensitivity: event1 or event2 @ " << sc_time_stamp() << std::endl;
    }
  }
};


int sc_main (int argc, char* argv[])
{
    SENSITIVITY sensitivity("sensitivity");
    sc_start(7, SC_SEC);
    return 0;
}