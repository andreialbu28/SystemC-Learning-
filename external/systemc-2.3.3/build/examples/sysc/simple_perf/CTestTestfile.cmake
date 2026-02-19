# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/simple_perf
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/simple_perf
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/simple_perf/simple_perf "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/simple_perf/simple_perf" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/simple_perf" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/simple_perf/simple_perf PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/simple_perf/CMakeLists.txt;44;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/simple_perf/CMakeLists.txt;0;")
