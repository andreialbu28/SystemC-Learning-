#include "systemc.h"
using namespace sc_core ; 

SC_MODULE ( turn_of_events )
{
    sc_event signal_stop, signals_off ; 
    sc_event stop_indicator_on, stop_indicator_off ; 

    SC_CTOR( turn_of_events)
    {
        SC_THREAD( turn_knob_thread) ;
        SC_THREAD (stop_signal_thread) ;
    }

    void turn_knob_thread(void)
    {
        enum directions {STOP = 'S',OFF = 'F'};
        char direction; // selects appropriate indicator
        bool did_stop = false ; // allow other threads to get into waiting state

        wait(SC_ZERO_TIME) ;

        while (true)
        {
            std::cout <<"Signal Command: " ;
            std::cin >> direction ;
            
            switch (direction)
            {
                case STOP:
                    signals_off.notify() ;
                    signal_stop.notify() ;
                    wait(stop_indicator_on) ;
                    did_stop = true ;
                    break;

                case OFF: 
                    signals_off.notify() ;
                    if (did_stop) wait(stop_indicator_off) ;
                    did_stop = false ;
                    break ; 
            }
        }
    }

    void stop_signal_thread(void)
    {
        while(true)
        {
            wait(signal_stop) ;
            std::cout << "STOPPING  !!!!!!!!" << std::endl ;

            stop_indicator_on.notify() ;
            wait(signals_off) ;
            std::cout << "Stop OFF  -------" << std::endl ;
            stop_indicator_off.notify() ;
        }
    }
};



int sc_main(int argc, char* argv[])
{
    turn_of_events events("events");
    sc_start() ;
    return 0 ; 
}