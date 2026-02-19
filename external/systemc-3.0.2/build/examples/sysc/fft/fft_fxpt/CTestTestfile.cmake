# CMake generated Testfile for 
# Source directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/fft/fft_fxpt
# Build directory: /home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/fft/fft_fxpt
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test([=[examples/sysc/fft/fft_fxpt/fft_fxpt/Release]=] "/usr/bin/cmake" "-DTEST_EXE=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/fft/fft_fxpt/fft_fxpt" "-DTEST_DIR=/home/andrei28/TCAD_journal/external/systemc-3.0.2/build/examples/sysc/fft/fft_fxpt" "-DTEST_INPUT=" "-DTEST_GOLDEN=/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/fft/fft_fxpt/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/home/andrei28/TCAD_journal/external/systemc-3.0.2/cmake/run-example.cmake")
set_tests_properties([=[examples/sysc/fft/fft_fxpt/fft_fxpt/Release]=] PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/CMakeLists.txt;101;add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/fft/fft_fxpt/CMakeLists.txt;74;configure_and_add_test;/home/andrei28/TCAD_journal/external/systemc-3.0.2/examples/sysc/fft/fft_fxpt/CMakeLists.txt;0;")
