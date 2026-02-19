#include "delta_cycle.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation - initialize variables and register threads
DELTA_CYCLE::DELTA_CYCLE(sc_module_name name)
    : x(1), y(1)  // Initialize x and y to 1
{
    SC_THREAD(add_x);
    SC_THREAD(multiply_x);
    SC_THREAD(add_y);
    SC_THREAD(multiply_y);
    
    cout << "Constructor: All 4 threads registered for " << name << endl;
    cout << "Initial values: x = " << x << ", y = " << y << endl;
    cout << "==========================================" << endl;
}

// Add 2 to x
void DELTA_CYCLE::add_x(void) {
    cout << "ADD X: " << x << " + 2 = ";
    x += 2;
    cout << x << endl;
}

// Multiply x by 2 (not by 3 as the message suggests - there's a bug!)
void DELTA_CYCLE::multiply_x(void) {
    cout << "MULTIPLY X: " << x << " * 3 = ";
    x *= 2;  // BUG: Should be *= 3 to match message, but shows delta cycle effects
    cout << x << endl;
}

// Add 2 to y
void DELTA_CYCLE::add_y(void) {
    cout << "ADD Y: " << y << " + 2 = ";
    y += 2;
    cout << y << endl;
}

// Multiply y by 2 (not by 3 as the message suggests)
void DELTA_CYCLE::multiply_y(void) {
    cout << "MULTIPLY Y: " << y << " * 3 = ";
    y *= 2;  // BUG: Should be *= 3 to match message
    cout << y << endl;
}