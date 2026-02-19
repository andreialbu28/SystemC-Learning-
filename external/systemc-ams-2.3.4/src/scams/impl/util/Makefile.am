
include $(top_srcdir)/config/Make-rules

SUBDIRS = \
	data_types \
	reporting \
	tracing

EXTRA_DIST += \
	CMakeLists.txt
	
INCLUDE_DIR = $(prefix)/include/scams/impl/util

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-rmdir $(DESTDIR)$(INCLUDE_DIR)

## Taf!
