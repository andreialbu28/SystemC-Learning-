file(GLOB SPARSE_LIBRARY_SOURCE 
	ana_init.c
	ana_reinit.c
	ana_solv.c
	ana_utilities.c
	linear_direct_sparse.c
	MA_generate_sparse.c
	MA_lequspar.c
	MA_matfull.c
	MA_matspars.c
	MA_LUdecomposition.c
	sca_solve_ac_linear.c)

add_library(sparse_library STATIC ${SPARSE_LIBRARY_SOURCE})

set_target_properties(sparse_library PROPERTIES LINKER_LANGUAGE CXX)