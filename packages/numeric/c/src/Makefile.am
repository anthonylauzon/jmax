packagedir=$(pkgdatadir)/packages/numeric/c
package_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES= \
numeric.c \
rewrap.c \
wrap.c 

noinst_HEADERS= \
numeric.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libnumeric_la_LDFLAGS=$(XLDFLAGS)
libnumeric_la_LIBADD=$(XLIBADD)

