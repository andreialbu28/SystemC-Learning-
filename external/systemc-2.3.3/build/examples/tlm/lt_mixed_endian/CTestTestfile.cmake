# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/tlm/lt_mixed_endian
# Build directory: /usr/local/systemc-2.3.3/build/examples/tlm/lt_mixed_endian
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/tlm/lt_mixed_endian/lt_mixed_endian "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/tlm/lt_mixed_endian/lt_mixed_endian" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/tlm/lt_mixed_endian" "-DTEST_INPUT=/usr/local/systemc-2.3.3/examples/tlm/lt_mixed_endian/results/input.txt" "-DTEST_GOLDEN=/usr/local/systemc-2.3.3/examples/tlm/lt_mixed_endian/results/expected.log" "-DTEST_FILTER=::" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/tlm/lt_mixed_endian/lt_mixed_endian PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/tlm/lt_mixed_endian/CMakeLists.txt;61;configure_and_add_test;/usr/local/systemc-2.3.3/examples/tlm/lt_mixed_endian/CMakeLists.txt;0;")
