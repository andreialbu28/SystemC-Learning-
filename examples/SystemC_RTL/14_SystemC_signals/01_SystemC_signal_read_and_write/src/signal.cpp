#include "../include/signal.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation
SIGNAL::SIGNAL(sc_module_name name) : sc_module(name) {
    SC_THREAD(readwrite);
    
    cout << "Constructor: Thread registered for " << name << endl;
    cout << "==========================================" << endl;
}

// Thread demonstrating signal behavior with delta cycles
void SIGNAL::readwrite(void) {
    // First write operation
    s.write(3);
    cout << "After s.write(3):" << endl;
    cout << "  s (direct) = " << s << endl;
    cout << "  s.read() = " << s.read() << endl;
    
    // Wait one delta cycle - signal updates become visible
    wait(SC_ZERO_TIME);
    cout << "After delta cycle:" << endl;
    cout << "  s = " << s << endl;
    
    // Multiple writes in same delta cycle
    cout << "\nMultiple writes in same delta cycle:" << endl;
    s = 4;  // First write
    cout << "  After s = 4 (immediate read): " << s << endl;
    s = 5;  // Second write - replaces the first
    cout << "  After s = 5 (immediate read): " << s << endl;
    
    // Read into temporary variable
    int tmp = s;
    cout << "  tmp = s: tmp = " << tmp << endl;
    
    // Wait another delta cycle
    wait(SC_ZERO_TIME);
    cout << "After delta cycle:" << endl;
    cout << "  s.read() = " << s.read() << endl;
}