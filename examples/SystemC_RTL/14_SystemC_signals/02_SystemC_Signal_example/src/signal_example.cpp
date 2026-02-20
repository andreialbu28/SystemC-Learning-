#include "../include/signal_example.h"
#include <iostream>
#include <string>

using namespace sc_core;
using std::cout;
using std::endl;

// Constructor implementation
signal_example::signal_example(sc_module_name name) : sc_module(name) {
    SC_THREAD(signal_example_thread);
    SC_METHOD(signal_example_method);

    // Static sensitivity for the method
    sensitive << count_sig
              << traffic_sig
              << message_signal;

    // Prevent method from triggering at time 0
    dont_initialize();

    cout << "Constructor: Thread and Method registered for " << name << endl;
    cout << "Method is statically sensitive to count_sig, traffic_sig, and message_signal" << endl;
    cout << "dont_initialize() called - method won't run at time 0" << endl;
    cout << "==========================================" << endl;
}

// Thread process - drives signal changes
void signal_example::signal_example_thread() {
    // Initializing the first delta cycle
    cout << "\n--- FIRST DELTA CYCLE ---" << endl;
    cout << "NOTE: signal_example_thread is initializing" << endl;

    // Write initial values to signals
    count_sig.write(10);
    traffic_sig.write(BLACK);
    message_signal.write("Hello");

    // Update local variables
    count = 10;
    traffic_temp = RED;
    message_temp = "Whoa";

    // End first delta cycle - signals will update
    wait(SC_ZERO_TIME);

    // Second delta cycle
    cout << "\n--- SECOND DELTA CYCLE ---" << endl;
    cout << "NOTE: signal_example_thread is done initializing" << endl;

    // Update local and signal
    count = 20;
    count_sig.write(count);
    cout << "---: count is " << count << " "
         << "count signal is " << count_sig.read() << endl;

    cout << "NOTE: signal_example_thread is waiting " << endl;

    // End second delta cycle
    wait(SC_ZERO_TIME);

    // Third delta cycle
    cout << "\n--- THIRD DELTA CYCLE ---" << endl;
    cout << "---: count is " << count << " "
         << "count signal is " << count_sig.read() << endl;

    // Update traffic signal
    traffic_sig.write(traffic_temp = WHITE);
    cout << "---: traffic_temp is " << traffic_temp << " "
         << "traffic sig is " << traffic_sig.read() << endl;

    // Update message signal
    message_signal.write(message_temp = "Rev your engines");
    cout << "---: message_temp is " << message_temp << " "
         << "traffic sig is " << traffic_sig.read() << endl;

    // End third delta cycle
    wait(SC_ZERO_TIME);

    // Fourth delta cycle
    cout << "\n--- FOURTH DELTA CYCLE ---" << endl;

    // End fourth delta cycle
    wait(SC_ZERO_TIME);

    // Fifth delta cycle
    cout << "\n--- FIFTH DELTA CYCLE ---" << endl;
    cout << "NOTE: signal_ex_thread done " << endl;
}

// Method process - triggered by signal changes
void signal_example::signal_example_method() {
    cout << "NOTE: signal_example_method detected an EVENT " << endl;

    cout << "...: count is " << count << " "
         << "and count signal is " << count_sig.read() << endl;

    cout << "---: traffic_temp is " << traffic_temp << " "
         << "and traffic sig is " << traffic_sig.read() << endl;

    cout << "---: message_temp is " << message_temp << " "
         << "and message signal is " << message_signal.read() << endl;
}