file(GLOB USER_SOLVER_SOURCE 
	sca_generic_user_solver.cpp)

add_library(user_solver STATIC ${USER_SOLVER_SOURCE})

set_target_properties(user_solver PROPERTIES LINKER_LANGUAGE CXX )