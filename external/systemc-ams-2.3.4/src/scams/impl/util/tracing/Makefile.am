
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libtracing.la

H_FILES = \
	sca_sc_trace.h \
	sca_trace_buffer.h \
	sca_trace_file_base.h \
	sca_trace_object_data.h \
	sca_trace_value_handler_base.h \
	sca_trace_value_handler.h \
	sca_type_explorer.h \
	sca_trace_value_handler_mm.h

NO_H_FILES = \
	sca_tabular_trace.h \
	sca_vcd_trace.h 

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_sc_trace.cpp \
	sca_tabular_trace.cpp \
	sca_tabular_trace_file.cpp \
	sca_trace.cpp \
	sca_trace_buffer.cpp \
	sca_trace_file_base.cpp \
	sca_trace_file.cpp \
	sca_trace_mode.cpp \
	sca_trace_object_data.cpp \
	sca_trace_value_handler_base.cpp \
	sca_trace_value_handler.cpp \
	sca_trace_write_comment.cpp \
	sca_vcd_trace.cpp \
	sca_vcd_trace_file.cpp \
	sca_trace_value_handler_mm.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libtracing_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/util/tracing

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
