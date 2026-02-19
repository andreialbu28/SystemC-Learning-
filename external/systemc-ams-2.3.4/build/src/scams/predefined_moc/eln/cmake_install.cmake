# Install script for directory: /home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/scams/predefined_moc/eln" TYPE FILE FILES
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_c.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_ccc.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_cccs.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_ccl.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_ccr.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_ccvs.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_gyrator.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_ideal_transformer.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_isource.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_l.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_module.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_node_if.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_node_ref.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_node.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_nullor.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_r.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_c.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_isink.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_isource.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_l.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_r.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_rswitch.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_vsink.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_sc_vsource.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_c.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_isink.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_isource.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_l.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_r.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_rswitch.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_vsink.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_tdf_vsource.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_terminal.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_transmission_line.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vcc.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vccs.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vcl.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vcr.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vcvs.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/predefined_moc/eln/sca_eln_vsource.h"
    )
endif()

