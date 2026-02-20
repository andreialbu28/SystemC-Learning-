#include "../include/signal_multi_writer.h"
#include <iostream>

using namespace sc_core;
using std::cout;
using std::endl;

// Constructor implementation
MULTI::MULTI(sc_module_name name) : sc_module(name) {
    // Register writer threads
    SC_THREAD(writer_1);
    SC_THREAD(writer_2);
    
    // Register consumer 1 - sensitive only to s1
    SC_THREAD(consumer_1);
    sensitive << s1;
    dont_initialize();
    
    // Register consumer 2 - sensitive to both s1 and s2
    SC_THREAD(consumer_2);
    sensitive << s1 << s2;
    dont_initialize();
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "s1: default mode (single writer recommended)" << endl;
    cout << "s2: SC_MANY_WRITERS mode (multiple writers allowed)" << endl;
    cout << "Consumer 1: sensitive to s1 only" << endl;
    cout << "Consumer 2: sensitive to s1 and s2" << endl;
    cout << "==========================================" << endl;
}

// Writer 1 - writes to both s1 and s2 every second
void MULTI::writer_1(void) {
    int v = 10;
    
    while (true) {
        s1.write(v);
        s2.write(v);
        
        cout << "@ time " << sc_time_stamp() << ": WRITER 1 writes: " << v++ << endl;
        wait(1, SC_SEC);
    }
}

// Writer 2 - writes only to s2 every second (decrementing value)
void MULTI::writer_2(void) {
    int v = 15;
    
    while (true) {
        wait(SC_ZERO_TIME);  // Ensure writer_1's write happens first in same delta
        s2.write(v);
        
        cout << "@ time " << sc_time_stamp() << ": WRITER 2 writes: " << v-- << endl;
        wait(1, SC_SEC);
    }
}

// Consumer 1 - sensitive only to s1, wakes when s1 changes
void MULTI::consumer_1(void) {
    while (true) {
        cout << "@ time " << sc_time_stamp() << " CONSUMER 1 reads s1 = " << s1.read() 
             << "; s2 = " << s2.read() << endl;
        wait();  // Wait for s1 event
    }
}

// Consumer 2 - sensitive to both s1 and s2, wakes when either changes
void MULTI::consumer_2(void) {
    while (true) {
        cout << "@ time " << sc_time_stamp() << " CONSUMER 2 reads s1 = " << s1.read() 
             << "; s2 = " << s2.read() << endl;
        wait();  // Wait for s1 or s2 event
    }
}