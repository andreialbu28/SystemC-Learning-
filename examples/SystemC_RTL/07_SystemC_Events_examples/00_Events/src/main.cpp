#include "../include/ordered_events.h"

using namespace sc_core;

int sc_main(int argc, char* argv[]) {
    // Instantiate the module
    ordered_events order_ev("order_ev");
    
    std::cout << "=== Ordered Events Circular Dependency Example ===" << std::endl;
    std::cout << "Event chain: A → B → C → A" << std::endl;
    std::cout << "A notifies a_event, waits for c_event" << std::endl;
    std::cout << "B notifies b_event, waits for a_event" << std::endl;
    std::cout << "C notifies c_event, waits for b_event" << std::endl;
    std::cout << "==================================================" << std::endl;
    
    // Run simulation for 3 nanoseconds
    sc_start(15, SC_NS);
    
    std::cout << "==================================================" << std::endl;
    std::cout << "Simulation finished at " << sc_time_stamp() << std::endl;
    
    return 0;
}