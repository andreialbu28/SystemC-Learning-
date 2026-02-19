
include $(top_srcdir)/config/Make-rules

EXTRA_DIST += \
	CMakeLists.txt

lib_LTLIBRARIES = libsystemc-ams.la

libsystemc_ams_ladir = 

libsystemc_ams_la_SOURCES = 

libsystemc_ams_la_LIBADD = \
	impl/core/libcore.la \
	impl/analysis/ac/libac_analysis.la \
	impl/predefined_moc/conservative/libconservative.la \
	impl/predefined_moc/eln/libeln.la \
	impl/predefined_moc/lsf/liblsf.la \
	impl/predefined_moc/tdf/libtdf.la \
	impl/solver/linear/liblinear_solver.la \
	impl/solver/tdf/libtdf_solver.la \
	impl/solver/user_solver/libuser_solver.la \
	impl/solver/util/sparse_library/libsparse_library.la \
	impl/synchronization/libsynchronization.la \
	impl/util/data_types/libdata_types.la \
	impl/util/reporting/libreporting.la \
	impl/util/tracing/libtracing.la

libsystemc_ams_la_LDFLAGS = -release $(PACKAGE_VERSION) -L$(libdir)

# dummy C++ source file to force C++ linking
nodist_EXTRA_libsystemc_ams_la_SOURCES = dummy.cpp


pkgconfigdir = $(libdir)/pkgconfig

pkgconfig_DATA = systemc-ams.pc


SUBDIRS = \
	core \
	analysis \
	predefined_moc \
	utility \
	impl

INCLUDE_DIR = $(prefix)/include/scams

install-data-local:
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(INCLUDE_DIR)

uninstall-local: uninstall-libLTLIBRARIES uninstall-pkgconfigDATA
	@$(NORMAL_UNINSTALL)
	-rmdir $(DESTDIR)$(INCLUDE_DIR)
	-rmdir $(DESTDIR)$(pkgconfigdir)
	-rmdir $(DESTDIR)$(libdir)

## Taf!
