file(GLOB CONSERVATIVE_SOURCE
    sca_con_interactive_trace_data.cpp 
	sca_conservative_module.cpp
	sca_conservative_signal.cpp
	sca_conservative_view.cpp)

add_library(conservative STATIC ${CONSERVATIVE_SOURCE})

install_headers( 
	sca_conservative_module.h
    sca_conservative_interface.h
    sca_conservative_signal.h)