#include "systemc.h"
#include <iostream>
#include <string>

using namespace sc_core ;

unsigned errors = 0 ; 
char* simulation_name = "gas_station" ;

SC_MODULE (gas_station)
{
    // Local Module Data
    const sc_core::sc_time t_MIN ; 
    bool m_filling ; 
    double m_full1, m_full2 ; 
    double m_filltime;
    double m_tank1, m_tank2;
    unsigned m_count1, m_count2, m_maxcount ;
    
    sc_event e_request1, e_request2 ; 
    sc_event tank_filled ; 

    SC_CTOR (gas_station)
    {
        SC_THREAD(customer1_thread) ; 
            sensitive(e_tank_filled) ;
        
        SC_METHOD(attendant_method);
            sensitive << e_request1 << e_request2 ; // streaming notation 

        SC_THREAD(customer2_thread);
    }

    void attentant_method(void)
    {
        

        if (!m_filling)
        {

        }
    }
};


int sc_main(int argc, char* argv[])
{
    sc_set_time_resolution(1,SC_NS) ;
    sc_set_default_time_unit(1,SC_NS) ;

    gas_station Charlies("Charlies",/*full1*/10,/*full2*/12,/*fulltime*/1.5,/*maxfills*/10) ;

    std::cout << " INFO: Starting gas_station simulation" <<endl ;

    sc_start() ; 

    std::cout << "INFO: Simulation " << simulation_name
       << " " << (errors?"FAILED":"PASSED")
       << " with " << errors << " errors"
       << endl;

    return errors?1:0;
}