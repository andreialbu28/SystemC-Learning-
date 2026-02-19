#----------------------------------------------------------------
# Generated CMake target import file for configuration "Release".
#----------------------------------------------------------------

# Commands may need to know the format version.
set(CMAKE_IMPORT_FILE_VERSION 1)

# Import target "SystemC::systemc" for configuration "Release"
set_property(TARGET SystemC::systemc APPEND PROPERTY IMPORTED_CONFIGURATIONS RELEASE)
set_target_properties(SystemC::systemc PROPERTIES
  IMPORTED_LOCATION_RELEASE "${_IMPORT_PREFIX}/lib/libsystemc.so.2.3.3"
  IMPORTED_SONAME_RELEASE "libsystemc.so.2.3"
  )

list(APPEND _cmake_import_check_targets SystemC::systemc )
list(APPEND _cmake_import_check_files_for_SystemC::systemc "${_IMPORT_PREFIX}/lib/libsystemc.so.2.3.3" )

# Commands beyond this point should not need to know the version.
set(CMAKE_IMPORT_FILE_VERSION)
