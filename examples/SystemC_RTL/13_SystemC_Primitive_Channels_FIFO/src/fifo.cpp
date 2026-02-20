#include "../include/fifo.h"
#include <iostream>

using namespace sc_core;
using namespace std;

// Constructor implementation - initialize all three FIFOs with size 2
FIFO::FIFO(sc_module_name name)
    : f1(2), f2(2), f3(2)  // Initialize each FIFO with capacity of 2
{
    // FIFO 1 - Blocking write/read
    SC_THREAD(generator1);
    SC_THREAD(consumer1);
    
    // FIFO 2 - Non-blocking with events
    SC_THREAD(generator2);
    SC_THREAD(consumer2);
    
    // FIFO 3 - Monitoring free/available slots
    SC_THREAD(generator3);
    SC_THREAD(consumer3);
    
    cout << "Constructor: All threads registered for " << name << endl;
    cout << "Three FIFO channels created, each with size 2" << endl;
    cout << "==========================================" << endl;
}

// ==================== FIFO 1: Blocking Write/Read ====================

// Generator 1 - Uses blocking write (write every 1 second)
void FIFO::generator1(void) {
    int v = 20;
    while (true) {
        f1.write(v);  // Blocking write - will wait if FIFO is full
        cout << "@ time " << sc_time_stamp() << ": GENERATOR 1 (using f1.write) writes " << v++ << endl;
        wait(1, SC_SEC);  // Write every 1 second
    }
}

// Consumer 1 - Uses blocking read (read every 3 seconds - slower than generator)
void FIFO::consumer1(void) {
    int v = -1;
    while (true) {
        f1.read(v);  // Blocking read - will wait if FIFO is empty
        cout << "@ time " << sc_time_stamp() << ": CONSUMER 1 (using f1.read) reads " << v << endl;
        wait(3, SC_SEC);  // Read every 3 seconds (slower than generator - FIFO will fill up)
    }
}

// ==================== FIFO 2: Non-blocking with Events ====================

// Generator 2 - Uses non-blocking write with event waiting
void FIFO::generator2(void) {
    int v = 0;
    while (true) {
        // Try non-blocking write until successful
        while (f2.nb_write(v) == false) {
            // If write fails (FIFO full), wait for data read event
            wait(f2.data_read_event());  // A slot becomes available when data is read
        }
        
        cout << sc_time_stamp() << ": GENERATOR 2 (using f2.nb_write(v)) writes " << v++ << endl;
        wait(1, SC_SEC);  // Write every 1 second
    }
}

// Consumer 2 - Uses non-blocking read with event waiting
void FIFO::consumer2(void) {
    int v = -1;
    while (true) {
        // Try non-blocking read until successful
        while (f2.nb_read(v) == false) {
            // If read fails (FIFO empty), wait for data written event
            wait(f2.data_written_event());  // Data becomes available when written
        }
        
        cout << sc_time_stamp() << ": CONSUMER 2 (using f2.nb_read(v)) reads " << v << endl;
        wait(3, SC_SEC);  // Read every 3 seconds (slower than generator)
    }
}

// ==================== FIFO 3: Monitoring Free/Available Slots ====================

// Generator 3 - Shows free/available slots before and after write
void FIFO::generator3(void) {
    int v = 0;
    while (true) {
        cout << sc_time_stamp() << ": GENERATOR 3, before write, #free/#available=" 
             << f3.num_free() << "/" << f3.num_available() << endl;
        
        f3.write(v++);  // Blocking write
        
        cout << sc_time_stamp() << ": GENERATOR 3, after write, #free/#available=" 
             << f3.num_free() << "/" << f3.num_available() << endl;
        
        wait(1, SC_SEC);  // Write every 1 second
    }
}

// Consumer 3 - Shows free/available slots before and after read
void FIFO::consumer3(void) {
    int v = -1;
    while (true) {
        cout << sc_time_stamp() << ": CONSUMER 3, before read, #free/#available=" 
             << f3.num_free() << "/" << f3.num_available() << endl;
        
        f3.read(v);  // Blocking read
        
        cout << sc_time_stamp() << ": CONSUMER 3, after read, #free/#available=" 
             << f3.num_free() << "/" << f3.num_available() << endl;
        
        wait(3, SC_SEC);  // Read every 3 seconds (slower than generator)
    }
}