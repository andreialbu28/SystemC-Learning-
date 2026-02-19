
include $(top_srcdir)/config/Make-rules

noinst_LTLIBRARIES = libac_analysis.la

H_FILES = 

NO_H_FILES = \
	sca_ac_domain_db.h \
	sca_ac_domain_entity.h \
	sca_ac_domain_globals.h \
	sca_ac_domain_eq.h \
	sca_ac_domain_solver.h

noinst_HEADERS = $(H_FILES)

CXX_FILES = \
	sca_ac_domain_db.cpp \
	sca_ac_domain_entity.cpp \
	sca_ac_domain_eq.cpp \
	sca_ac_domain_solver.cpp \
	sca_ac_domain_globals.cpp \
	sca_ac_start.cpp \
	sca_ac_noise_start.cpp \
	sca_ac_functions.cpp \
	sca_ac_object.cpp

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

libac_analysis_la_SOURCES = $(H_FILES) $(NO_H_FILES) $(CXX_FILES) 

INCLUDE_DIR = $(prefix)/include/scams/impl/analysis/ac

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
