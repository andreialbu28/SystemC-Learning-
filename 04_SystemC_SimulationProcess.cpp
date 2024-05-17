#include "systemc.h"

using namespace sc_core; 

SC_MODULE ( PROCESS )
{
    sc_clock clk ; // declaring a clock

    SC_CTOR (PROCESS) : clk ("clk",1,SC_SEC) // instantiate a clock with 1 second periodicity
    {
        SC_METHOD(method); // register a method 
        SC_THREAD(thread); // register a thread 
        SC_CTHREAD(cthread, clk); // registered a clocked thread

    }

    void method(void)  // defines the method member function, no while loop here 
    {
        std::cout << "Method triggered @ " << sc_time_stamp() << std :: endl ; 
        next_trigger(sc_time(1,SC_SEC)) ;  // trigger after 1 second
    }

    void thread (void)  // defines the thread member function, infinite loop make sure it never exits
    {
        while (true)
        {
            std::cout << "Thread triggered @ " << sc_time_stamp() << std :: endl ;
            wait(500,SC_MS) ; // wait 500 milliseconds before execute again
        }
    }

    void cthread (void)
    {
        while (true)
        {
            std::cout << "Cthread triggered @ " << sc_time_stamp() << std::endl ; 

            wait(); // wait for next clock event, which comes after 1 second ( defined in line 9)
        }
    }
};


int sc_main (int argc, char* argv[])
{
    PROCESS process("process");
    
    std::cout << " Execution phase begins @ " << sc_time_stamp() << std::endl ; 

    sc_start(5,SC_SEC);

    std :: cout << "Execution time ends @ " << sc_time_stamp() << std::endl ;

    return 0 ;
}