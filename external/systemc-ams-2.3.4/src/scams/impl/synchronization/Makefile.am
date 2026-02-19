
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libsynchronization.la

H_FILES = \
	sca_sync_value_handle.h

NO_H_FILES = \
	sca_synchronization_alg.h \
	sca_synchronization_layer_process.h \
	sca_synchronization_layer.h \
	sca_synchronization_obj_if.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_sync_value_handle.cpp \
	sca_synchronization_alg.cpp \
	sca_synchronization_layer_process.cpp \
	sca_synchronization_layer.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libsynchronization_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/synchronization

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
