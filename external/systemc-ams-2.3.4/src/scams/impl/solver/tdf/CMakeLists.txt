file(GLOB SOLVERTDF_SOURCE 
	sca_tdf_solver.cpp)

add_library(solvertdf STATIC ${SOLVERTDF_SOURCE})