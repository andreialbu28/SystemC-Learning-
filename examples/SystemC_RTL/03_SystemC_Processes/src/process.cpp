#include "../include/process.h"
#include <iostream>

// Constructor implementation with initializer list
// The clock is instantiated here with name "clk", period 1 second
PROCESS::PROCESS(sc_core::sc_module_name name)
    : clk("clk", 1, SC_SEC)  // Initialize clock in initializer list
{
    // Register all three process types with the kernel
    SC_METHOD(method);        // Method process - runs to completion
    SC_THREAD(thread);        // Thread process - can suspend/resume
    SC_CTHREAD(cthread, clk); // Clocked thread - synchronous to clock
    
    std::cout << "Constructor of " << name << " executed" << std::endl;
}

// Method process implementation
// SC_METHOD runs to completion, cannot have while loop
void PROCESS::method(void) {
    std::cout << "Method triggered @ " << sc_time_stamp() << std::endl;
    // Schedule next trigger after 1 second
    next_trigger(sc_time(1, SC_SEC));
}

// Thread process implementation
// SC_THREAD has its own stack, can suspend with wait()
void PROCESS::thread(void) {
    while (true) {
        std::cout << "Thread triggered @ " << sc_time_stamp() << std::endl;
        // Wait for 500 ms before next execution
        wait(500, SC_MS);
    }
}

// Clocked thread implementation
// SC_CTHREAD is synchronized to clock edges
void PROCESS::cthread(void) {
    while (true) {
        std::cout << "Cthread triggered @ " << sc_time_stamp() << std::endl;
        // Wait for next clock event (1 second period as defined)
        wait();  // waits for next clock edge
    }
}