/*
        sc_buffer is a predefined primitive channel derived from class sc_signal.

        It differs from class sc_signal in that a value-changed event is notified whenever the buffer is written rather than only when the value of the signal is changed. 
        E.g.,
        if current value of a "signal" == 1: write 1 to it won't trigger value update event.
        if current value of a "buffer" == 1: write 1 to it will trigger value update event.
*/

#include "systemc.h"
#include <iostream>

using namespace sc_core ; 
using std::cout ; 
using std::endl ;

SC_MODULE ( buffer_ex )
{
    sc_signal<double> my_sig ; 
    sc_signal<double> my_buf ;
    
    SC_CTOR ( buffer_ex )
    {
        SC_THREAD (buffer_ex_thread);

        SC_METHOD (signal_method) ; 
        sensitive << my_sig ;
        dont_initialize();

        SC_METHOD(buffer_method);
        sensitive << my_buf ; 
        dont_initialize();
    }

    void buffer_ex_thread()
    {
        const sc_time t_1N(1,SC_NS);

        cout << "NOTE ( buffer_ex_thread): Initializing" << endl ;

        my_sig.write(1.1) ;
        my_buf.write(2.1) ;
        wait (5*t_1N) ; 

        cout << "NOTE ( buffer_ex_thread): Rewriting " << endl ; 
        my_sig.write(1.1) ;
        my_buf.write(2.1) ;
        wait (5*t_1N) ;

        cout << "NOTE ( buffer_ex_thread): Changing " << endl ; 
        my_sig.write(1.2) ;
        my_buf.write(2.1) ;
        wait (5*t_1N) ;

        cout << "NOTE ( buffer_ex_thread): Done" << endl ; 
    }

    
    void signal_method ()
    {
        cout << "INFO: my_signal = " << my_sig.read() << " @ time " << sc_time_stamp() << endl;
    }

    void buffer_method()
    {
        cout << "INFO: my_buffer = " << my_buf.read() << " @  time" << sc_time_stamp() << endl;
    }
};


unsigned errors = 0 ; 
char* simulation_name = "buffer example" ; 

int sc_main ( int argc, char* argv[])
{
    cout << " INFO: Elaborating " << simulation_name << endl ;

    // sc_set_time_resolution(1,SC_PS);
    // sc_set_default_time_unit(1,SC_NS);

    buffer_ex buffer_ex_i("buffer_ex_i");

    cout << "INFO: Simulating " << simulation_name << endl ;
    sc_start();

    cout << "INFO: Post-processing "<< simulation_name << endl;
    cout << "INFO: Simulation " << simulation_name << " " << (errors?"FAILED":"PASSED") << " with " << errors << " errors" << endl;

    return errors?1:0;
}