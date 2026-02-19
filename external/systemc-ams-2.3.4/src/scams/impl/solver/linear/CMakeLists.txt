file(GLOB LINEAR_SOURCE
	sca_linear_solver.cpp)

add_library(linear STATIC ${LINEAR_SOURCE})

install_headers(
	sca_linear_solver_parameters.h)