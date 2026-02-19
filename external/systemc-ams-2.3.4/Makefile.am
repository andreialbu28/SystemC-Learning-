
include $(top_srcdir)/config/Make-rules

SUBDIRS = \
	docs \
	src

EXTRA_DIST += \
	AUTHORS \
	COPYING \
	ChangeLog \
	INSTALL \
	LICENSE \
	NEWS \
	NOTICE \
	README \
	RELEASENOTES \
	msvc12 \
    CMakeLists.txt

debug:
if !WANT_DEBUG
	$(MAKE) $(AM_MAKEFLAGS) clean
	cd $(top_builddir) && $(SHELL) $(top_srcdir)/configure --with-arch-suffix=$(LIB_ARCH_SUFFIX) --enable-debug --disable-optimized --no-create --no-recursion
endif
	$(MAKE) $(AM_MAKEFLAGS) all

opt:
if !WANT_OPTIMIZE
	$(MAKE) $(AM_MAKEFLAGS) clean
	cd $(top_builddir) && $(SHELL) $(top_srcdir)/configure --with-arch-suffix=$(LIB_ARCH_SUFFIX) --disable-debug --enable-optimized --no-create --no-recursion
endif
	$(MAKE) $(AM_MAKEFLAGS) all


## Taf!
