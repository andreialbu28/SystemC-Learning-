#include "systemc.h"
using namespace sc_core ; 

SC_MODULE (MULTI)
{
    sc_signal<int> s1; 
    sc_signal<int, SC_MANY_WRITERS> s2 ;

    SC_CTOR (MULTI)
    {
        SC_THREAD(writer_1) ;
        SC_THREAD(writer_2) ;

        SC_THREAD(consumer_1) ;
        sensitive << s1 ; 
        dont_initialize() ; 

        SC_THREAD(consumer_2);
        sensitive << s1 << s2 ;
        dont_initialize() ;
    }

    void writer_1()
    {
        int v = 10 ; 

        while (true)
        {
            s1.write(v) ;
            s2.write(v) ;

            std::cout  << "@ time" << sc_time_stamp() << ": WRITER 1 writes: " << v++ << std::endl ; 
            wait(1,SC_SEC);

        }
    }

    void writer_2()
    {
        int v = 15 ; 

        while(true)
        {
            wait(SC_ZERO_TIME);
            s2.write(v); 
            
            std::cout  << "@ time" << sc_time_stamp() << ": WRITER 2 writes: " << v-- << std::endl ; 
            wait(1,SC_SEC);
        }
    }

    void consumer_1 ()
    {
        while(true)
        {
            std::cout  << "@ time" << sc_time_stamp() << " CONSUMER 1 reads s1= " << s1.read() << "; s2 = " << s2.read() << std::endl ;
            wait();  // wait for s1
        }
    }

    void consumer_2 ()
    {
        while(true)
        {
            std::cout  << "@ time" << sc_time_stamp() << " CONSUMER 2 reads s1= " << s1.read() << "; s2 = " << s2.read() << std::endl ;
            wait();  // wait for s1 or s2
        }
    }

};

int sc_main(int argc, char* argv[])
{
    MULTI multi("multi");
    sc_start(3,SC_SEC);
    return 0; 
}