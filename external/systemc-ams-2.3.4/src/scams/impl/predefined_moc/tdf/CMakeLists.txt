file(GLOB TDF_SOURCE
	sca_ct_delay_buffer.cpp
	sca_delay_buffer_base.cpp
	sca_tdf_ct_ltf_nd_proxy.cpp
	sca_tdf_ct_ltf_zp_proxy.cpp
	sca_tdf_ct_vector_ss_proxy.cpp
	sca_tdf_default_interpolator.cpp
	sca_tdf_ltf_nd.cpp
	sca_tdf_ltf_zp.cpp
	sca_tdf_module.cpp
	sca_tdf_port_attributes.cpp
	sca_tdf_sc_in_bool.cpp
	sca_tdf_sc_in_sc_logic.cpp
	sca_tdf_signal_impl_base.cpp
	sca_tdf_ss.cpp
	sca_tdf_trace_variable_base.cpp
	sca_tdf_view.cpp)

add_library(impl_tdf STATIC ${TDF_SOURCE})

install_headers(
	sca_tdf_port_impl.h
    sca_tdf_port_attributes.h
    sca_tdf_signal_impl.h
    sca_tdf_signal_impl_base.h
    sca_tdf_trace_variable_base.h
    sca_dt_delay_buffer.h
    sca_delay_buffer_base.h)