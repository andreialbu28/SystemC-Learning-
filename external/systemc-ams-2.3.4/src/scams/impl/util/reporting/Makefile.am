
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libreporting.la

H_FILES = 

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_information.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libreporting_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/util/reporting

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
