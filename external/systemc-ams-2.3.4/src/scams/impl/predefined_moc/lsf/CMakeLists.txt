file(GLOB LSF_SOURCE 
	sca_lsf_add.cpp
	sca_lsf_de_demux.cpp
    sca_lsf_de_gain.cpp
    sca_lsf_de_mux.cpp
    sca_lsf_de_sink.cpp
    sca_lsf_de_source.cpp
	sca_lsf_delay.cpp
	sca_lsf_dot.cpp
	sca_lsf_gain.cpp
    sca_lsf_in.cpp
    sca_lsf_integ.cpp
    sca_lsf_lsf_demux.cpp
    sca_lsf_lsf_gain.cpp
    sca_lsf_lsf_mux.cpp
    sca_lsf_ltf_nd.cpp
    sca_lsf_ltf_zp.cpp
    sca_lsf_module.cpp
    sca_lsf_out.cpp
    sca_lsf_signal.cpp
    sca_lsf_source.cpp
    sca_lsf_ss.cpp
    sca_lsf_sub.cpp
	sca_lsf_tdf_demux.cpp
	sca_lsf_tdf_gain.cpp
	sca_lsf_tdf_mux.cpp
	sca_lsf_tdf_sink.cpp
	sca_lsf_tdf_source.cpp
	sca_lsf_view.cpp)

add_library(impl_lsf STATIC ${LSF_SOURCE})