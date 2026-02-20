#ifndef FIFO_H
#define FIFO_H

#include <systemc.h>
#include <iostream>

SC_MODULE(FIFO) {
    // Three FIFO channels with size 2 each
    sc_fifo<int> f1, f2, f3;
    
    // Constructor declaration
    SC_CTOR(FIFO);
    
private:
    // Generator and consumer threads for FIFO 1 (blocking)
    void generator1(void);
    void consumer1(void);
    
    // Generator and consumer threads for FIFO 2 (non-blocking with events)
    void generator2(void);
    void consumer2(void);
    
    // Generator and consumer threads for FIFO 3 (monitoring free/available slots)
    void generator3(void);
    void consumer3(void);
};

#endif