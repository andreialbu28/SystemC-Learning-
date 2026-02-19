# CMake generated Testfile for 
# Source directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/at_4_phase
# Build directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/at_4_phase
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[examples/tlm/at_4_phase/at_4_phase/Release]=] "/usr/bin/cmake" "-DTEST_EXE=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/at_4_phase/at_4_phase" "-DTEST_DIR=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/at_4_phase" "-DTEST_INPUT=" "-DTEST_GOLDEN=/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/at_4_phase/results/expected.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/home/andrei28/TCAD_journal/external/systemc-3.0.2/cmake/run-example.cmake")
set_tests_properties([=[examples/tlm/at_4_phase/at_4_phase/Release]=] PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/CMakeLists.txt;101;add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/at_4_phase/CMakeLists.txt;59;configure_and_add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/at_4_phase/CMakeLists.txt;0;")
