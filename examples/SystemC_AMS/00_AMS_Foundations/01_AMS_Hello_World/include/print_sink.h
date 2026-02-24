// =============================================================================
// print_sink.h
// A TDF sink module that reads samples and prints them.
//
// Demonstrates:
//   - sca_tdf::sca_in<T> input port
//   - Reading a value inside processing() with in.read()
// =============================================================================
#include "systemc-ams"
using namespace sc_core;

SCA_TDF_MODULE(PrintSink) {

    sca_tdf::sca_in<double> in;

    SCA_CTOR(PrintSink);

    void set_attributes() override;
    
    void processing() override;

private:
    int sample_count;
};