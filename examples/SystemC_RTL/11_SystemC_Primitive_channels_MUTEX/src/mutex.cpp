#include "../include/mutex.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation
MUTEX::MUTEX(sc_module_name name) : sc_module(name) {
    SC_THREAD(thread_1);
    SC_THREAD(thread_2);
    SC_THREAD(thread_3);
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Mutex m created for resource access control" << endl;
    cout << "==========================================" << endl;
}

// Thread 1 - Tries to lock mutex every 1 second
void MUTEX::thread_1(void) {
    int number = 0;  // Unused but kept for consistency with original
    
    while (true) {
        // First try to lock without blocking
        if (m.trylock() == -1) { 
            cout << " - THREAD_1 --> Trying to lock the mutex but fails, returns -1 " << endl;
            m.lock();  // Block until mutex is available
            cout << sc_time_stamp() << ": thread_1 obtained resource by lock()" << endl;
        } else { 
            cout << " - THREAD_1 --> Trying to lock the mutex and succeeded, returns 0" << endl;
            cout << sc_time_stamp() << ": thread_1 obtained resource by trylock()" << endl;
        }

        // Use the resource for 1 second
        wait(1, SC_SEC);
        
        // Release the mutex
        m.unlock();
        cout << sc_time_stamp() << ": unlocked by thread_1" << endl;
        
        // Give other threads a chance to lock in the same delta cycle
        wait(SC_ZERO_TIME);
    }
}

// Thread 2 - Tries to lock mutex every 3 seconds
void MUTEX::thread_2(void) {
    while (true) {
        // First try to lock without blocking
        if (m.trylock() == -1) { 
            cout << " -- THREAD_2 --> Trying to lock the mutex but fails, returns -1 " << endl;
            m.lock();  // Block until mutex is available
            cout << sc_time_stamp() << ": thread_2 obtained resource by lock()" << endl;
        } else { 
            cout << " -- THREAD_2 --> Trying to lock the mutex and succeeded, returns 0" << endl;
            cout << sc_time_stamp() << ": thread_2 obtained resource by trylock()" << endl;
        }
        
        // Use the resource for 3 seconds
        wait(3, SC_SEC);
        
        // Release the mutex
        m.unlock();
        cout << sc_time_stamp() << ": unlocked by thread_2" << endl;
        
        // Give other threads a chance to lock in the same delta cycle
        wait(SC_ZERO_TIME);
    }
}

// Thread 3 - Tries to lock mutex every 5 seconds
void MUTEX::thread_3(void) {
    while (true) {
        // First try to lock without blocking
        if (m.trylock() == -1) { 
            cout << " --- THREAD_3 --> Trying to lock the mutex but fails, returns -1 " << endl;
            m.lock();  // Block until mutex is available
            cout << sc_time_stamp() << ": thread_3 obtained resource by lock()" << endl;
        } else { 
            cout << " --- THREAD_3 --> Trying to lock the mutex and succeeded, returns 0" << endl;
            cout << sc_time_stamp() << ": thread_3 obtained resource by trylock()" << endl;
        }
        
        // Use the resource for 5 seconds
        wait(5, SC_SEC);
        
        // Release the mutex
        m.unlock();
        cout << sc_time_stamp() << ": unlocked by thread_3" << endl;
        
        // Give other threads a chance to lock in the same delta cycle
        wait(SC_ZERO_TIME);
    }
}