
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libdata_types.la

H_FILES = \
	sca_function_vector.h \
	sca_matrix_base.h \
	sca_matrix_base_complex.h \
	sca_matrix_base_double.h \
	sca_matrix_base_typeless.h \
	sca_method_list.h \
	sca_method_object.h \
	convert_from_string.h \
	sca_lock_free_queue.h

NO_H_FILES = \
	sca_change_matrix.h \
	sca_pwl_stamps.h \
	almost_equal.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_change_matrix.cpp \
	sca_matrix_base_complex.cpp \
	sca_matrix_base_double.cpp \
	sca_matrix_base_typeless.cpp \
	sca_pwl_stamps.cpp \
	almost_equal.cpp \
	convert_from_string.cpp \
	sca_lock_free_queue.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libdata_types_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/util/data_types

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
