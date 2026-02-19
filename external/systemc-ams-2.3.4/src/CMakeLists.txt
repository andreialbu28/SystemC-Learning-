include_directories(${CMAKE_SOURCE_DIR}/src)

add_subdirectory(scams)

get_target_property(IMPL_AC_SOURCE impl_ac SOURCES)
get_target_property(CORE_SOURCE core SOURCES)
get_target_property(CONSERVATIVE_SOURCE conservative SOURCES)
get_target_property(ELN_SOURCE eln SOURCES)
get_target_property(IMPL_LSF_SOURCE impl_lsf SOURCES)
get_target_property(IMPL_TDF_SOURCE impl_tdf SOURCES)
get_target_property(LINEAR_SOURCE linear SOURCES)
get_target_property(SOLVERTDF_SOURCE solvertdf SOURCES)
get_target_property(USER_SOLVER_SOURCE user_solver SOURCES)
get_target_property(SPARSE_LIBRARY_SOURCE sparse_library SOURCES)
get_target_property(SYNCHRONIZATION_SOURCE synchronization SOURCES)
get_target_property(DATA_TYPES_SOURCE data_types SOURCES)
get_target_property(REPORTING_SOURCE reporting SOURCES)
get_target_property(TRACING_SOURCE tracing SOURCES)

add_library(systemc-ams STATIC 
	${IMPL_AC_SOURCE}
	${CORE_SOURCE}
	${CONSERVATIVE_SOURCE}
	${ELN_SOURCE}
	${IMPL_LSF_SOURCE}
	${IMPL_TDF_SOURCE}
	${LINEAR_SOURCE}
	${SOLVERTDF_SOURCE}
	${USER_SOLVER_SOURCE}
	${SPARSE_LIBRARY_SOURCE}
	${SYNCHRONIZATION_SOURCE}
	${DATA_TYPES_SOURCE}
	${REPORTING_SOURCE}
	${TRACING_SOURCE})
	

install_headers(systemc-ams	systemc-ams.h config.h DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

install(TARGETS systemc-ams RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
                         LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
                         ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR})