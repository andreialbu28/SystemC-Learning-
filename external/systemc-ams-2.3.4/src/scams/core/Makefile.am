
include $(top_srcdir)/config/Make-rules


H_FILES = \
	sca_assign_from_proxy.h \
	sca_assign_to_proxy.h \
	sca_interface.h \
	sca_implementation_info.h \
	sca_module.h \
	sca_parameter_base.h \
	sca_parameter.h \
	sca_port.h \
	sca_prim_channel.h \
	sca_time.h \
	sca_max_time.h \
	sca_set_default_solver_parameter.h \
	sca_user_solver_base.h \
	sca_physical_domain_interface.h

NO_H_FILES = 

noinst_HEADERS = $(H_FILES)

CXX_FILES = 

NO_CXX_FILES =

EXTRA_DIST += \
	CMakeLists.txt

INCLUDE_DIR = $(prefix)/include/scams/core

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
