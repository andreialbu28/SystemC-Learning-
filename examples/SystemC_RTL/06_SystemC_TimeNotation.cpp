#include "systemc.h"

using namespace sc_core ; 

int sc_main(int argc, char* argv[])
{

    sc_core :: sc_report_handler::set_actions ("/IEEE_Std_1666/deprecated",sc_core::SC_DO_NOTHING); // suppress warning due to set_time_resolution

    sc_set_time_resolution(1,SC_FS);

    sc_set_default_time_unit(1,SC_SEC);

    std::cout << " 1 SEC =      " << sc_time(1,SC_SEC).to_default_time_units() << " SEC" << std::endl ; 

    std::cout << " 1 SEC =      " << sc_time(1,SC_MS).to_default_time_units() << " SEC" << std::endl ; 

    std::cout << " 1 SEC =      " << sc_time(1,SC_US).to_default_time_units() << " SEC" << std::endl ; 

    std::cout << " 1 SEC =      " << sc_time(1,SC_NS).to_default_time_units() << " SEC" << std::endl ; 

    std::cout << " 1 SEC =      " << sc_time(1,SC_PS).to_default_time_units() << " SEC" << std::endl ; 

    std::cout << " 1 SEC =      " << sc_time(1,SC_FS).to_default_time_units() << " SEC" << std::endl ; 

    sc_start(10000,SC_SEC) ; 

    double t = sc_time_stamp().to_seconds(); // get time in second

    std::cout << int(t) / 3600 << " hours, " << (int(t)%3600)/60 << "minutes, " << (int(t) % 60) << "seconds" << std::endl ;

    return 0; 
}

