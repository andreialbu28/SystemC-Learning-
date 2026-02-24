// =============================================================================
// main.cpp — 01_ams_hello_world
//
// The simplest possible SystemC AMS simulation:
//   HelloTDF (source) --[sca_tdf::sca_signal]--> PrintSink (sink)
//
// Key things to observe when running:
//   1. The AMS modules print at 1 µs intervals even though there are no
//      sc_signal events — the AMS scheduler drives them on a fixed clock.
//   2. sc_start() launches BOTH the digital event kernel and the AMS scheduler.
//   3. The program prints exactly 5 lines then the simulation ends.
// =============================================================================
#include "../include/hello_tdf.h"
#include "../include/print_sink.h"

int sc_main(int, char**) {

    // -------------------------------------------------------------------------
    // Channel: connects one TDF output to one TDF input.
    // sca_tdf::sca_signal<T> is the AMS equivalent of sc_signal<T>.
    // It is NOT an sc_signal — you cannot bind a regular sc_out to it.
    // -------------------------------------------------------------------------
    sca_tdf::sca_signal<double> sig("sig");

    // -------------------------------------------------------------------------
    // Module instantiation — identical syntax to standard SystemC.
    // -------------------------------------------------------------------------
    HelloTDF  source("source");
    PrintSink sink("sink");

    // -------------------------------------------------------------------------
    // Port binding — also identical to standard SystemC.
    // -------------------------------------------------------------------------
    source.out(sig);
    sink.in(sig);

    // -------------------------------------------------------------------------
    // sc_start() runs both the digital kernel and the AMS scheduler together.
    // Here we run for 5 µs: the 1 MHz TDF clock fires at t=0,1,2,3,4 µs.
    // -------------------------------------------------------------------------
    std::cout << "\n=== AMS Hello World ===\n\n";
    sc_core::sc_start(5, SC_US);
    std::cout << "\nSimulation finished at t = "
              << sc_core::sc_time_stamp() << "\n";

    return 0;
}