# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/rsa
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/rsa
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/rsa/rsa "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/rsa/rsa" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/rsa" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/rsa/rsa PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/rsa/CMakeLists.txt;44;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/rsa/CMakeLists.txt;0;")
