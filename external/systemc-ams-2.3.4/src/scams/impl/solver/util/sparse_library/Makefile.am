
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libsparse_library.la

H_FILES = 

NO_H_FILES = \
	ana_solv_data.h \
	ma_sparse.h \
	ma_typedef.h \
	ma_util.h \
	linear_analog_solver.h \
	linear_direct_sparse.h \
	sca_solve_ac_linear.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	ana_init.c \
	ana_reinit.c \
	ana_solv.c \
	ana_utilities.c \
	MA_generate_sparse.c \
	MA_lequspar.c \
	MA_matfull.c \
	MA_matspars.c \
	MA_LUdecomposition.c \
	linear_direct_sparse.c \
	sca_solve_ac_linear.c

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libsparse_library_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/solver/util/sparse_library

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
