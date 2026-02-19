# CMake generated Testfile for 
# Source directory: /usr/local/systemc-2.3.3/examples/sysc/fft/fft_fxpt
# Build directory: /usr/local/systemc-2.3.3/build/examples/sysc/fft/fft_fxpt
# 
# This file includes the relevant testing commands required for 
# testing this directory and lists subdirectories to be tested as well.
add_test(examples/sysc/fft/fft_fxpt/fft_fxpt "/usr/bin/cmake" "-DTEST_EXE=/usr/local/systemc-2.3.3/build/examples/sysc/fft/fft_fxpt/fft_fxpt" "-DTEST_DIR=/usr/local/systemc-2.3.3/build/examples/sysc/fft/fft_fxpt" "-DTEST_INPUT=" "-DTEST_GOLDEN=/usr/local/systemc-2.3.3/examples/sysc/fft/fft_fxpt/golden.log" "-DTEST_FILTER=" "-DDIFF_COMMAND=/usr/bin/diff" "-DDIFF_OPTIONS=-u" "-P" "/usr/local/systemc-2.3.3/cmake/run_test.cmake")
set_tests_properties(examples/sysc/fft/fft_fxpt/fft_fxpt PROPERTIES  FAIL_REGULAR_EXPRESSION "^[*][*][*]ERROR" _BACKTRACE_TRIPLES "/usr/local/systemc-2.3.3/examples/CMakeLists.txt;137;add_test;/usr/local/systemc-2.3.3/examples/sysc/fft/fft_fxpt/CMakeLists.txt;74;configure_and_add_test;/usr/local/systemc-2.3.3/examples/sysc/fft/fft_fxpt/CMakeLists.txt;0;")
