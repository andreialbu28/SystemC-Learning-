##
##
##

include $(top_srcdir)/config/Make-rules


DOC_FILES = 

## add documentation to distribution
EXTRA_DIST += \
	$(DOC_FILES)


install-data-local:
	@$(NORMAL_INSTALL)
	if test "$(realpath $(srcdir))" != "$(realpath $(DESTDIR)$(docdir))"; then \
	  $(mkinstalldirs) $(DESTDIR)$(docdir); \
	  for file in $(DOC_FILES); do \
	    $(INSTALL_DATA) -D $(srcdir)/$$file $(DESTDIR)$(docdir)/$$file; \
	  done; \
	fi

uninstall-local:
	@$(NORMAL_UNINSTALL)
	if test "$(realpath $(srcdir))" != "$(realpath $(DESTDIR)$(docdir))"; then \
	  for file in $(DOC_FILES); do \
	    rm -f $(DESTDIR)$(docdir)/$$file; \
	  done; \
	  rmdir $(DESTDIR)$(docdir); \
	fi

## Taf!
