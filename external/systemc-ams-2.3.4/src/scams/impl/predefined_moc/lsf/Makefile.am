
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = liblsf.la

H_FILES = 

NO_H_FILES = \
	sca_lsf_view.h 

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_lsf_module.cpp \
	sca_lsf_view.cpp \
	sca_lsf_signal.cpp \
	sca_lsf_in.cpp \
	sca_lsf_out.cpp \
	sca_lsf_add.cpp \
	sca_lsf_dot.cpp \
	sca_lsf_tdf_source.cpp \
	sca_lsf_gain.cpp \
	sca_lsf_integ.cpp \
	sca_lsf_sub.cpp \
	sca_lsf_tdf_sink.cpp \
	sca_lsf_de_demux.cpp \
	sca_lsf_de_gain.cpp \
	sca_lsf_de_mux.cpp \
	sca_lsf_de_source.cpp \
	sca_lsf_de_sink.cpp \
	sca_lsf_tdf_mux.cpp \
	sca_lsf_tdf_demux.cpp \
	sca_lsf_tdf_gain.cpp \
	sca_lsf_ss.cpp \
	sca_lsf_ltf_nd.cpp \
	sca_lsf_ltf_zp.cpp \
	sca_lsf_delay.cpp \
	sca_lsf_source.cpp \
	sca_lsf_lsf_gain.cpp \
	sca_lsf_lsf_demux.cpp \
	sca_lsf_lsf_mux.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

liblsf_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/predefined_moc/lsf

#install-data-local:
#	@$(NORMAL_INSTALL)
#	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)
#	for file in $(noinst_HEADERS); do \
#	  $(INSTALL_DATA) $(srcdir)/$$file $(DESTDIR)$(INCLUDE_DIR)/$$file; \
#	done

#uninstall-local:
#	@$(NORMAL_UNINSTALL)
#	for file in $(noinst_HEADERS); do \
#	  rm -f $(DESTDIR)$(INCLUDE_DIR)/$$file; \
#	done
#	-rmdir $(DESTDIR)$(INCLUDE_DIR)

## Taf!
