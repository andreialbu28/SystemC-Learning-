
include $(top_srcdir)/config/Make-rules


H_FILES = \
	sca_ac_functions.h \
	sca_ac_noise_start.h \
	sca_ac_start.h \
	sca_ac_object.h \
	sca_ac_module.h

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = 

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

INCLUDE_DIR = $(prefix)/include/scams/analysis/ac

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
