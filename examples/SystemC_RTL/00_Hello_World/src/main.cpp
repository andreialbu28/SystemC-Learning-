#include "../include/hello_world.h"

using namespace sc_core;

// Normal C++ function (not a SystemC process)
void hello1() {
    std::cout << "Hello world using Normal C++ function" << std::endl;
}

// SystemC entry point
int sc_main(int argc, char* argv[]) {
    // Approach #1: Directly call a normal C++ function
    hello1();
    
    // Approach #2: Instantiate and simulate a SystemC module
    HelloWorld helloworld("helloworld");  // Create module instance
    sc_start();  // Start simulation - kernel will call helloworld.hello2()
    
    return 0;
}