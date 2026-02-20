#include "../include/semaphore.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation - initialize semaphore with 2 resources
SEMAPHORE::SEMAPHORE(sc_module_name name)
    : s(2)  // Initialize semaphore with 2 available resources
{
    SC_THREAD(thread_1);
    SC_THREAD(thread_2);
    SC_THREAD(thread_3);
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Semaphore initialized with " << s.get_value() << " available resources" << endl;
    cout << "==========================================" << endl;
}

// Thread 1 - Holds resource for 2 seconds
void SEMAPHORE::thread_1(void) {
    while (true) {
        // Try to obtain a resource without blocking
        if (s.trywait() == -1) { 
            cout << sc_time_stamp() << ": THREAD 1 : resource not available, value of semaphore is " << s.get_value() << endl;
            s.wait(); // Block until a resource becomes available
        }

        cout << sc_time_stamp() << ": locked by thread_1, value is " << s.get_value() << endl;
        wait(2, SC_SEC); // Occupy resource for 2 seconds
        s.post(); // Release the resource
        cout << sc_time_stamp() << ": unlocked by thread_1, value is " << s.get_value() << endl;
        wait(SC_ZERO_TIME); // Give other processes a chance to lock in same delta cycle
    }
}

// Thread 2 - Holds resource for 1 second
void SEMAPHORE::thread_2(void) {
    while (true) {
        // Try to obtain a resource without blocking
        if (s.trywait() == -1) { 
            cout << sc_time_stamp() << ": THREAD 2 : resource not available, value of semaphore is " << s.get_value() << endl;
            s.wait(); // Block until a resource becomes available
        }

        cout << sc_time_stamp() << ": locked by thread_2, value is " << s.get_value() << endl;
        wait(1, SC_SEC); // Occupy resource for 1 second
        s.post(); // Release the resource
        cout << sc_time_stamp() << ": unlocked by thread_2, value is " << s.get_value() << endl;
        wait(SC_ZERO_TIME); // Give other processes a chance to lock in same delta cycle
    }
}

// Thread 3 - Holds resource for 1 second
void SEMAPHORE::thread_3(void) {
    while (true) {
        // Try to obtain a resource without blocking
        if (s.trywait() == -1) { 
            cout << sc_time_stamp() << ": THREAD 3 : resource not available, value of semaphore is " << s.get_value() << endl;
            s.wait(); // Block until a resource becomes available
        }

        cout << sc_time_stamp() << ": locked by thread_3, value is " << s.get_value() << endl;
        wait(1, SC_SEC); // Occupy resource for 1 second
        s.post(); // Release the resource
        cout << sc_time_stamp() << ": unlocked by thread_3, value is " << s.get_value() << endl;
        wait(SC_ZERO_TIME); // Give other processes a chance to lock in same delta cycle
    }
}