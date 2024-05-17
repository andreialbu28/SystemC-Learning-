#include "systemc.h"
#include <iostream>
#include <string>

using namespace sc_core ;
using std::cout ; 
using std::endl ;


SC_MODULE ( signal_example )
{
    enum color{ BLACK, RED, GREEN, BLUE, YELLOW, MAGENTA, CYAN, WHITE };

    // Local Data Variables
    int count ; 
    color traffic_temp ; 
    std::string message_temp ; 

    // Local channels
    sc_signal<int> count_sig ;
    sc_signal<color> traffic_sig ;
    sc_signal<std::string> message_signal ; 

    SC_CTOR(signal_example)
    {
        SC_THREAD ( signal_example_thread );
        SC_METHOD ( signal_example_method );

        sensitive << count_sig
                  << traffic_sig
                  << message_signal ; 
        
        dont_initialize() ; // ensures that the method does not get triggered at time 0 
    }

    void signal_example_thread()
    {
        // Initializing the first delta cycle 
        cout << "--- FIRST DELTA CYCLE ---" << endl ;
        cout << "NOTE: signal_example_thread is initializing" << endl ; 

        count_sig.write(10) ; 
        traffic_sig.write(BLACK);
        message_signal.write("Hello");

        count = 10 ; 
        traffic_temp = RED ; 
        message_temp = "Whoa" ; 
        wait(SC_ZERO_TIME);

        // Second delta cycle 
        cout << "--- SECOND DELTA CYCLE ---" << endl ;
        cout << "NOTE: signal_example_thread is done initializing" << endl ;

        count = 20 ;
        count_sig.write(count);
        cout << "---: count is " << count << " "
             << "count signal is " << count_sig.read() << endl ;

        cout << "NOTE: signal_example_thread is waiting " << endl ;

        // third delta cycle
        wait(SC_ZERO_TIME);
        cout << "--- THIRD DELTA CYCLE ---" << endl ;
        cout << "---: count is " << count << " "
             << "count signal is " << count_sig.read() << endl ;

        traffic_sig.write(traffic_temp = WHITE);
        cout << "---: traffic_temp is " << traffic_temp << " "
             << "traffic sig is " << traffic_sig.read() << endl ;

        message_signal.write(message_temp="Rev your engines");
        cout << "---: traffic_temp is " << traffic_temp << " "
             << "traffic sig is " << traffic_sig.read() << endl ;

        // 4th delta cycle
        wait(SC_ZERO_TIME);
        cout << "--- FORTH DELTA CYCLE ---" << endl ;

        // 5th delta cycle
        wait(SC_ZERO_TIME);
        cout << "--- FIFTH DELTA CYCLE ---" << endl ;
        cout << "NOTE: signal_ex_thread done " << endl ; 
    }

    void signal_example_method()
    {
        cout << "NOTE : signal_example_method detected an EVENT " << endl ; 

        cout << "...: count is " << count << " " 
             << "and count signal is " << count_sig.read() << endl ;

         cout << "---: traffic_temp is " << traffic_temp << " "
              << "and traffic sig is " << traffic_sig.read() << endl ;

        cout << "---: message_temp is " << message_temp << " " 
             << " and message signal is " << message_signal.read() << endl ; 
    }
};


unsigned errors = 0 ; 
char* simulation_name = " 20_02 Signal example " ;

int sc_main(int argc, char* argv[])
{
    cout << "INFO: Elaborating " << simulation_name << endl ; 

    sc_set_time_resolution(1,SC_PS) ; 
    sc_set_default_time_unit(1,SC_NS);

    signal_example singal_example_i("signal_example_i");

    cout << " INFO: Simulating " << simulation_name << endl ;
    sc_start() ; 
   
    cout << "INFO: Post-processing "<< simulation_name << endl;
    cout << "INFO: Simulation " << simulation_name
        << " " << (errors?"FAILED":"PASSED")
        << " with " << errors << " errors"
        << endl;
    return errors?1:0;

}