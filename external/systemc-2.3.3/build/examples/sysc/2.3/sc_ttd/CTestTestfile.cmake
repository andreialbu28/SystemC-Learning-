# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/2.3/sc_ttd
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/2.3/sc_ttd
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/2.3/sc_ttd/sc_ttd "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/2.3/sc_ttd/sc_ttd" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/2.3/sc_ttd" "-DTEST_INPUT=" "-DTEST_GOLDEN=/usr/local/systemc-2.3.3/examples/sysc/2.3/sc_ttd/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/2.3/sc_ttd/sc_ttd PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/2.3/sc_ttd/CMakeLists.txt;45;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/2.3/sc_ttd/CMakeLists.txt;0;")
