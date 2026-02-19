#include "turn_of_events.h"

using namespace sc_core;
using namespace std;

int sc_main(int argc, char* argv[]) {
    cout << "=== Turn of Events Example ===" << endl;
    cout << "Simulating car turn signal behavior" << endl;
    cout << "==========================================\n" << endl;
    
    // Instantiate the module
    turn_of_events events("events");
    
    cout << "\nStarting interactive simulation..." << endl;
    cout << "Enter commands: S (STOP) or F (OFF)" << endl;
    cout << "Type Ctrl+C to exit" << endl;
    cout << "------------------------------------------\n" << endl;
    
    // Start simulation (runs until Ctrl+C)
    sc_start();
    
    // This point may never be reached if using Ctrl+C
    cout << "\n------------------------------------------" << endl;
    cout << "Simulation finished at " << sc_time_stamp() << endl;
    
    return 0;
}