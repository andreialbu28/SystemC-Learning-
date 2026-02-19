file(GLOB CORE_SOURCE 
	sca_globals.cpp
	sca_implementation_info.cpp
	sca_interface.cpp
	sca_max_time.cpp
	sca_module.cpp
	sca_object_manager.cpp
	sca_parameter_base.cpp
	sca_port_base.cpp
	sca_prim_channel.cpp
	sca_set_default_solver_parameter.cpp
	sca_signed_time.cpp
    sca_simcontext.cpp
	sca_solver_base.cpp
	sca_solver_manager.cpp
	sca_user_solver_base.cpp
	sca_view_base.cpp
	sca_view_manager.cpp
	sca_view_object.cpp)

add_library(core STATIC ${CORE_SOURCE})

install_headers(   
	sca_port_base.h
    sca_port_impl.h
    sca_signed_time.h
    sca_user_extensions.h)