
include $(top_srcdir)/config/Make-rules

SUBDIRS = \
	sparse_library
	
EXTRA_DIST += \
	CMakeLists.txt

INCLUDE_DIR = $(prefix)/include/scams/impl/solver/util

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-rmdir $(DESTDIR)$(INCLUDE_DIR)

## Taf!
