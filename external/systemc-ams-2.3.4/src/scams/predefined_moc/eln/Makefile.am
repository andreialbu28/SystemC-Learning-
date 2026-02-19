
include $(top_srcdir)/config/Make-rules


H_FILES = \
	sca_eln_c.h \
	sca_eln_cccs.h \
	sca_eln_ccvs.h \
	sca_eln_gyrator.h \
	sca_eln_ideal_transformer.h \
	sca_eln_isource.h \
	sca_eln_l.h \
	sca_eln_module.h \
	sca_eln_node_if.h \
	sca_eln_node_ref.h \
	sca_eln_node.h \
	sca_eln_nullor.h \
	sca_eln_r.h \
	sca_eln_sc_c.h \
	sca_eln_sc_isink.h \
	sca_eln_sc_isource.h \
	sca_eln_sc_l.h \
	sca_eln_sc_r.h \
	sca_eln_sc_rswitch.h \
	sca_eln_sc_vsink.h \
	sca_eln_sc_vsource.h \
	sca_eln_tdf_c.h \
	sca_eln_tdf_isink.h \
	sca_eln_tdf_isource.h \
	sca_eln_tdf_l.h \
	sca_eln_tdf_r.h \
	sca_eln_tdf_rswitch.h \
	sca_eln_tdf_vsink.h \
	sca_eln_tdf_vsource.h \
	sca_eln_terminal.h \
	sca_eln_transmission_line.h \
	sca_eln_vccs.h \
	sca_eln_vcvs.h \
	sca_eln_vsource.h \
	sca_eln_vcr.h \
	sca_eln_vcc.h \
	sca_eln_vcl.h \
	sca_eln_ccr.h \
	sca_eln_ccc.h \
	sca_eln_ccl.h

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = 

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

INCLUDE_DIR = $(prefix)/include/scams/predefined_moc/eln

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
