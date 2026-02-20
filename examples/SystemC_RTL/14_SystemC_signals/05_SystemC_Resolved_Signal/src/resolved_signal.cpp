#include "../include/resolved_signal.h"
#include <iostream>
#include <vector>

using namespace sc_core;
using namespace sc_dt;
using std::vector;
using std::cout;
using std::endl;

// Constructor implementation - initialize levels vector
RESOLVED_SIGNAL::RESOLVED_SIGNAL(sc_module_name name)
    : levels({sc_logic_0, sc_logic_1, sc_logic_Z, sc_logic_X})
{
    SC_THREAD(writer1);
    SC_THREAD(writer2);
    SC_THREAD(consumer);
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Resolved signal with 4-level logic (0,1,Z,X)" << endl;
    cout << "Writer1: cycles through 0,1,Z,X every second" << endl;
    cout << "Writer2: cycles through 0,0,0,0,1,1,1,1,Z,Z,Z,Z,X,X,X,X" << endl;
    cout << "Consumer: reads resolved value every second (starting at t=1s)" << endl;
    cout << "==========================================" << endl;
}

// Writer 1 - Cycles through 0,1,Z,X repeatedly
void RESOLVED_SIGNAL::writer1(void) {
    int idx = 0;
    while (true) {
        // Pattern: 0,1,Z,X,0,1,Z,X,0,1,Z,X,...
        rv.write(levels[idx++ % 4]);
        wait(1, SC_SEC);  // Write every 1 second
    }
}

// Writer 2 - Cycles through each value 4 times: 0,0,0,0,1,1,1,1,Z,Z,Z,Z,X,X,X,X
void RESOLVED_SIGNAL::writer2(void) {
    int idx = 0;
    while (true) {
        // Pattern: 4 of each value: 0x4, 1x4, Zx4, Xx4, repeat
        rv.write(levels[(idx++ / 4) % 4]);
        wait(1, SC_SEC);  // Write every 1 second
    }
}

// Consumer - Reads resolved value every second (starting at t=1s)
void RESOLVED_SIGNAL::consumer(void) {
    wait(1, SC_SEC);  // Delay first read by 1 second
    
    int idx = 0;
    while (true) {
        cout << " " << rv.read() << " |";  // Print resolved value
        
        if (++idx % 4 == 0) {
            cout << endl;  // New line every 4 values
        }
        
        wait(1, SC_SEC);  // Read every 1 second
    }
}