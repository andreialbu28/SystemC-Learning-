
include $(top_srcdir)/config/Make-rules


H_FILES = \
	sca_tdf_ct_proxy.h \
	sca_tdf_ct_vector_proxy.h \
	sca_tdf_default_interpolator.h \
	sca_tdf_in.h \
	sca_tdf_ltf_nd.h \
	sca_tdf_ltf_zp.h \
	sca_tdf_module.h \
	sca_tdf_out.h \
	sca_tdf_out_base.h \
	sca_tdf_out_ct_cut.h \
	sca_tdf_out_dt_cut.h \
	sca_tdf_sc_in.h \
	sca_tdf_sc_in_bool.h \
	sca_tdf_sc_in_sc_logic.h \
	sca_tdf_sc_out.h \
	sca_tdf_signal_if.h \
	sca_tdf_signal.h \
	sca_tdf_ss.h \
	sca_tdf_trace_variable.h

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = 

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt
	
INCLUDE_DIR = $(prefix)/include/scams/predefined_moc/tdf

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)
	for file in $(noinst_HEADERS); do \
	  $(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(INCLUDE_DIR)/$$file; \
	done

uninstall-local:
	@$(NORMAL_UNINSTALL)
	for file in $(noinst_HEADERS); do \
	  rm -f $(DESTDIR)$(INCLUDE_DIR)/$$file; \
	done
	-rmdir $(DESTDIR)$(INCLUDE_DIR)

## Taf!
