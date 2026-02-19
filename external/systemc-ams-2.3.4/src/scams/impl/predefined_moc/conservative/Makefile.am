
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libconservative.la

H_FILES = \
	sca_conservative_module.h \
	sca_conservative_interface.h \
	sca_conservative_signal.h

NO_H_FILES = \
	sca_conservative_view.h \
	sca_con_interactive_trace_data.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_conservative_module.cpp \
	sca_conservative_signal.cpp \
	sca_conservative_view.cpp \
	sca_con_interactive_trace_data.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libconservative_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/predefined_moc/conservative

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
