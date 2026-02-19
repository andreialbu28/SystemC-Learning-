file(GLOB DATA_TYPES_SOURCE 
    almost_equal.cpp
    convert_from_string.cpp
    sca_change_matrix.cpp
    sca_lock_free_queue.cpp
    sca_matrix_base_complex.cpp
    sca_matrix_base_double.cpp
    sca_matrix_base_typeless.cpp  
    sca_change_matrix.cpp
    sca_pwl_stamps.cpp)


add_library(data_types STATIC ${DATA_TYPES_SOURCE})

install_headers(
    convert_from_string.h
    sca_lock_free_queue.h
    sca_matrix_base_typeless.h
	sca_matrix_base.h
	sca_matrix_base_double.h
	sca_matrix_base_complex.h
	sca_method_list.h
	sca_method_object.h
	sca_function_vector.h)