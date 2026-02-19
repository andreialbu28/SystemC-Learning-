
include $(top_srcdir)/config/Make-rules

SUBDIRS = \
	core \
	analysis \
	predefined_moc \
	solver \
	synchronization \
	util
EXTRA_DIST += \
	CMakeLists.txt
	
INCLUDE_DIR = $(prefix)/include/scams/impl

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)

uninstall-local:
	@$(NORMAL_UNINSTALL)
	-rmdir $(DESTDIR)$(INCLUDE_DIR)

## Taf!
