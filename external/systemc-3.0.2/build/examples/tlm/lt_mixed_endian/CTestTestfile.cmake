# CMake generated Testfile for 
# Source directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/lt_mixed_endian
# Build directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/lt_mixed_endian
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[examples/tlm/lt_mixed_endian/lt_mixed_endian/Release]=] "/usr/bin/cmake" "-DTEST_EXE=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/lt_mixed_endian/lt_mixed_endian" "-DTEST_DIR=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/tlm/lt_mixed_endian" "-DTEST_INPUT=/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/lt_mixed_endian/results/input.txt" "-DTEST_GOLDEN=/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/lt_mixed_endian/results/expected.log" "-DTEST_FILTER=::" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/home/andrei28/TCAD_journal/external/systemc-3.0.2/cmake/run-example.cmake")
set_tests_properties([=[examples/tlm/lt_mixed_endian/lt_mixed_endian/Release]=] PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/CMakeLists.txt;101;add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/lt_mixed_endian/CMakeLists.txt;61;configure_and_add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/tlm/lt_mixed_endian/CMakeLists.txt;0;")
