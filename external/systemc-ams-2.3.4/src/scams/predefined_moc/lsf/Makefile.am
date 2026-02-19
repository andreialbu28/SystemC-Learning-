
include $(top_srcdir)/config/Make-rules


H_FILES = \
	sca_lsf_add.h \
	sca_lsf_delay.h \
	sca_lsf_dot.h \
	sca_lsf_gain.h \
	sca_lsf_in.h \
	sca_lsf_integ.h \
	sca_lsf_ltf_nd.h \
	sca_lsf_ltf_zp.h \
	sca_lsf_out.h \
	sca_lsf_de_demux.h \
	sca_lsf_de_gain.h \
	sca_lsf_de_mux.h \
	sca_lsf_de_sink.h \
	sca_lsf_de_source.h \
	sca_lsf_signal_if.h \
	sca_lsf_signal.h \
	sca_lsf_source.h \
	sca_lsf_ss.h \
	sca_lsf_sub.h \
	sca_lsf_tdf_demux.h \
	sca_lsf_tdf_gain.h \
	sca_lsf_tdf_mux.h \
	sca_lsf_tdf_sink.h \
	sca_lsf_tdf_source.h \
	sca_lsf_module.h \
	sca_lsf_lsf_gain.h \
	sca_lsf_lsf_demux.h \
	sca_lsf_lsf_mux.h

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = 

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

INCLUDE_DIR = $(prefix)/include/scams/predefined_moc/lsf

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
