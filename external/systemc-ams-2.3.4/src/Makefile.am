
include $(top_srcdir)/config/Make-rules


H_FILES = \
	systemc-ams \
	systemc-ams.h

NO_H_FILES = config.h

noinst_HEADERS = $(H_FILES)

CXX_FILES =

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt \
	config.h

SUBDIRS = \
	scams

INCLUDE_DIR = $(prefix)/include

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
