# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/simple_bus
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/simple_bus
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/simple_bus/simple_bus "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/simple_bus/simple_bus" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/simple_bus" "-DTEST_INPUT=" "-DTEST_GOLDEN=/usr/local/systemc-2.3.3/examples/sysc/simple_bus/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/simple_bus/simple_bus PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/simple_bus/CMakeLists.txt;66;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/simple_bus/CMakeLists.txt;0;")
