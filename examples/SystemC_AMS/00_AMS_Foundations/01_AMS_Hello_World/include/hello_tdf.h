// =============================================================================
// hello_tdf.h
// A minimal TDF module — the AMS equivalent of "Hello World".
//
// Demonstrates:
//   - SCA_TDF_MODULE and SCA_CTOR
//   - sca_tdf::sca_out<T> port
//   - set_attributes() and processing() callbacks
//   - set_timestep() to control execution rate
// =============================================================================
#include "systemc-ams"
using namespace sc_core;

// SCA_TDF_MODULE(Name) expands to:
//   struct Name : public sca_tdf::sca_module
// It is the AMS counterpart of SC_MODULE.
SCA_TDF_MODULE(HelloTDF) {

    // A TDF output port — produces one double sample per timestep.
    // sca_tdf::sca_out<T> is NOT an sc_out<T>; it connects to
    // sca_tdf::sca_signal<T> channels, not sc_signal<T>.
    sca_tdf::sca_out<double> out;

    // SCA_CTOR works like SC_CTOR — registers the module name with the kernel.
    SCA_CTOR(HelloTDF);

    // -------------------------------------------------------------------------
    // set_attributes() — called once during elaboration (before simulation).
    // This is the ONLY place to call set_timestep(), set_rate(), set_delay().
    // Do NOT call them from the constructor.
    // -------------------------------------------------------------------------
    void set_attributes() override;

    // -------------------------------------------------------------------------
    // processing() — called by the AMS scheduler every timestep.
    // Equivalent to an SC_METHOD body, but driven by the AMS clock, not events.
    // -------------------------------------------------------------------------
    void processing() override;

private:
    int sample_count;
};