#include "../include/hello_world.h"

// Constructor implementation
HelloWorld::HelloWorld(sc_module_name name) {
    SC_METHOD(hello2);  // Register hello2 as a method process
}

// Process function implementation
void HelloWorld::hello2() {
    std::cout << "Hello world using SystemC method process" << std::endl;
}
