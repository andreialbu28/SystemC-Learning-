# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/2.1/sc_export
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/2.1/sc_export
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/2.1/sc_export/sc_export "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/2.1/sc_export/sc_export" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/2.1/sc_export" "-DTEST_INPUT=" "-DTEST_GOLDEN=/usr/local/systemc-2.3.3/examples/sysc/2.1/sc_export/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/2.1/sc_export/sc_export PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/2.1/sc_export/CMakeLists.txt;44;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/2.1/sc_export/CMakeLists.txt;0;")
