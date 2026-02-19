# Install script for directory: /usr/local/systemc-2.3.3/examples/sysc

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/opt/systemc")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "Release")
  endif()
  message(STATUS "Install configuration: \"${CMAKE_INSTALL_CONFIG_NAME}\"")
endif()

# Set the component getting installed.
if(NOT CMAKE_INSTALL_COMPONENT)
  if(COMPONENT)
    message(STATUS "Install component: \"${COMPONENT}\"")
    set(CMAKE_INSTALL_COMPONENT "${COMPONENT}")
  else()
    set(CMAKE_INSTALL_COMPONENT)
  endif()
endif()

# Install shared libraries without execute permission?
if(NOT DEFINED CMAKE_INSTALL_SO_NO_EXE)
  set(CMAKE_INSTALL_SO_NO_EXE "1")
endif()

# Is this installation the result of a crosscompile?
if(NOT DEFINED CMAKE_CROSSCOMPILING)
  set(CMAKE_CROSSCOMPILING "FALSE")
endif()

# Set default install directory permissions.
if(NOT DEFINED CMAKE_OBJDUMP)
  set(CMAKE_OBJDUMP "/usr/bin/objdump")
endif()

if(NOT CMAKE_INSTALL_LOCAL_ONLY)
  # Include the install script for each subdirectory.
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/dpipe/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/forkjoin/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/reset_signal_is/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/sc_export/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/sc_report/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/scx_barrier/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/scx_mutex_w_policy/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.1/specialized_signals/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.3/sc_rvd/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.3/sc_ttd/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/2.3/simple_async/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/fft/fft_flpt/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/fft/fft_fxpt/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/fir/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/pipe/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/pkt_switch/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/risc_cpu/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/rsa/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/simple_bus/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/simple_fifo/cmake_install.cmake")
  include("/usr/local/systemc-2.3.3/build/examples/sysc/simple_perf/cmake_install.cmake")

endif()

