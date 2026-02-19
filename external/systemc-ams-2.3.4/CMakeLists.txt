###############################################################################
#
#    Copyright 2015-2023
#    COSEDA Technologies GmbH
#
#   Licensed under the Apache License, Version 2.0 (the "License");
#   you may not use this file except in compliance with the License.
#   You may obtain a copy of the License at
#
#       http://www.apache.org/licenses/LICENSE-2.0
#
#   Unless required by applicable law or agreed to in writing, software
#   distributed under the License is distributed on an "AS IS" BASIS,
#   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
#   See the License for the specific language governing permissions and
#   limitations under the License.
#
###############################################################################

###############################################################################
#  CMakeLists.txe - simple cmake flow
#  Original Author: Paul Ehrlich COSEDA Technologies GmbH
###############################################################################

cmake_minimum_required(VERSION 3.1)

project(systemc_ams CXX C)

###############################################################################
# Build options
###############################################################################

option (ENABLE_PARALLEL_TRACING "Enable parallel tracing and thus add a pthread dependency." OFF)
option (DISABLE_REFERENCE_NODE_CLUSTERING "Disables clustering for refrence nodes - reference nodes ignored for clustering." OFF)
option (DISABLE_PERFORMANCE_STATISTICS "Disables performance data collection and removes dependency from high precision counter and chrono" OFF)

mark_as_advanced(
        ENABLE_PARALLEL_TRACING
        DISABLE_REFERENCE_NODE_CLUSTERING
        DISABLE_PERFORMANCE_STATISTICS)

add_definitions(-D_USE_MATH_DEFINES)
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT ENABLE_PARALLEL_TRACING)
    add_compile_definitions(DISABLE_PARALLEL_TRACING)
endif(NOT ENABLE_PARALLEL_TRACING)  
if(DISABLE_REFERENCE_NODE_CLUSTERING)
    add_compile_definitions(DISABLE_REFERENCE_NODE_CLUSTERING)
endif(DISABLE_REFERENCE_NODE_CLUSTERING)    
if(DISABLE_PERFORMANCE_STATISTICS)
    add_compile_definitions(DISABLE_PERFORMANCE_STATISTICS)
endif(DISABLE_PERFORMANCE_STATISTICS)   



###############################################################################
# Configure status
###############################################################################

if (ENABLE_PARALLEL_TRACING)
  message ("ENABLE_PARALLEL_TRACING = ${ENABLE_PARALLEL_TRACING}")
else (ENABLE_PARALLEL_TRACING)
  message (STATUS "ENABLE_PARALLEL_TRACING = ${ENABLE_PARALLEL_TRACING}")
endif (ENABLE_PARALLEL_TRACING)

if (DISABLE_REFERENCE_NODE_CLUSTERING)
  message ("DISABLE_REFERENCE_NODE_CLUSTERING = ${DISABLE_REFERENCE_NODE_CLUSTERING}")
else (DISABLE_REFERENCE_NODE_CLUSTERING)
  message (STATUS "DISABLE_REFERENCE_NODE_CLUSTERING = ${DISABLE_REFERENCE_NODE_CLUSTERING}")
endif (DISABLE_REFERENCE_NODE_CLUSTERING)

if (DISABLE_PERFORMANCE_STATISTICS)
  message ("DISABLE_PERFORMANCE_STATISTICS = ${DISABLE_PERFORMANCE_STATISTICS}")
else (DISABLE_PERFORMANCE_STATISTICS)
  message (STATUS "DISABLE_PERFORMANCE_STATISTICS = ${DISABLE_PERFORMANCE_STATISTICS}")
endif (DISABLE_PERFORMANCE_STATISTICS)

include_directories($ENV{SYSTEMC_HOME}/include)

macro(install_headers)
	string(REPLACE ${CMAKE_SOURCE_DIR}/src "" REL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
	install(FILES ${ARGV} DESTINATION include/${REL_DIR})
endmacro(install_headers)

add_subdirectory(src)

install(
	FILES 
		AUTHORS
		ChangeLog
		COPYING
		INSTALL
		LICENSE
		NEWS
		NOTICE
		README
		RELEASENOTES
	DESTINATION .)
	