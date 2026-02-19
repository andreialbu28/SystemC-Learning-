#include "../include/concurrency.h"
#include <iostream>

using namespace sc_core;

// Constructor implementation - register both threads
CONCURRENCY::CONCURRENCY(sc_module_name name) : sc_module(name) {
    SC_THREAD(thread_1);  // Register first thread
    SC_THREAD(thread_2);  // Register second thread
    std::cout << "Constructor: Both threads registered for " << name << std::endl;
}

// Thread 1 implementation - runs every 2 seconds
void CONCURRENCY::thread_1(void) {
    while(true) {
        std::cout << sc_time_stamp() << ": thread 1 " << std::endl;
        wait(2, SC_SEC);  // Yield control for 2 seconds
    }
}

// Thread 2 implementation - runs every 4 seconds
void CONCURRENCY::thread_2(void) {
    while(true) {
        std::cout << "     " << sc_time_stamp() << ": thread 2 " << std::endl;
        wait(4, SC_SEC);  // Yield control for 4 seconds
    }
}