# Install script for directory: /home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf

# Set the install prefix
if(NOT DEFINED CMAKE_INSTALL_PREFIX)
  set(CMAKE_INSTALL_PREFIX "/usr/local/systemc-ams-2.3.4")
endif()
string(REGEX REPLACE "/$" "" CMAKE_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}")

# Set the install configuration name.
if(NOT DEFINED CMAKE_INSTALL_CONFIG_NAME)
  if(BUILD_TYPE)
    string(REGEX REPLACE "^[^A-Za-z0-9_]+" ""
           CMAKE_INSTALL_CONFIG_NAME "${BUILD_TYPE}")
  else()
    set(CMAKE_INSTALL_CONFIG_NAME "")
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

if(CMAKE_INSTALL_COMPONENT STREQUAL "Unspecified" OR NOT CMAKE_INSTALL_COMPONENT)
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/scams/predefined_moc/tdf" TYPE FILE FILES
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_ct_proxy.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_ct_vector_proxy.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_default_interpolator.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_in.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_ltf_nd.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_ltf_zp.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_module.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_out_base.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_out_ct_cut.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_out_dt_cut.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_out.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_sc_in_bool.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_sc_in_sc_logic.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_sc_in.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_sc_out.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_signal_if.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_signal.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_ss.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/tdf/sca_tdf_trace_variable.h"
    )
endif()

