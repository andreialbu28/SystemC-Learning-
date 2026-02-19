# CMake generated Testfile for 
# Source directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/rsa
# Build directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/rsa
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[examples/sysc/rsa/rsa/Release]=] "/usr/bin/cmake" "-DTEST_EXE=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/rsa/rsa" "-DTEST_DIR=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/rsa" "-DTEST_INPUT=" "-DTEST_GOLDEN=" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/home/andrei28/TCAD_journal/external/systemc-3.0.2/cmake/run-example.cmake")
set_tests_properties([=[examples/sysc/rsa/rsa/Release]=] PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/CMakeLists.txt;101;add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/rsa/CMakeLists.txt;44;configure_and_add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/rsa/CMakeLists.txt;0;")
