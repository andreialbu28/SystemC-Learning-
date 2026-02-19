# Install script for directory: /home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types

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
  file(INSTALL DESTINATION "${CMAKE_INSTALL_PREFIX}/include/scams/impl/util/data_types" TYPE FILE FILES
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/convert_from_string.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_lock_free_queue.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_matrix_base_typeless.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_matrix_base.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_matrix_base_double.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_matrix_base_complex.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_method_list.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_method_object.h"
    "/home/andrei28/systemc-ams-2.3.4/src/scams/impl/util/data_types/sca_function_vector.h"
    )
endif()

