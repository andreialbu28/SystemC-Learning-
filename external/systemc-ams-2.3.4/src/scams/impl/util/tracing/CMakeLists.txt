file(GLOB TRACING_SOURCE 
	sca_sc_trace.cpp
	sca_tabular_trace_file.cpp
	sca_tabular_trace.cpp
	sca_trace_buffer.cpp
	sca_trace_file_base.cpp
	sca_trace_file.cpp
	sca_trace_mode.cpp
	sca_trace_object_data.cpp
	sca_trace_value_handler_base.cpp
	sca_trace_value_handler_mm.cpp
	sca_trace_value_handler.cpp
	sca_trace_write_comment.cpp
	sca_trace.cpp
	sca_vcd_trace.cpp
	sca_vcd_trace_file.cpp)

add_library(tracing STATIC ${TRACING_SOURCE})

install_headers(
	sca_trace_object_data.h
    sca_trace_value_handler_base.h
    sca_trace_value_handler.h
    sca_trace_file_base.h
    sca_trace_buffer.h
    sca_sc_trace.h
    sca_trace_value_handler_mm.h
    sca_type_explorer.h
    )