packagedir=$(pkgdatadir)/packages/ispwmath/c
package_LTLIBRARIES=libispwmath.la
libispwmath_la_SOURCES= \
binop.c \
ispwmath.c 

noinst_HEADERS= \
ispwmath.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libispwmath_la_LDFLAGS=$(XLDFLAGS)
libispwmath_la_LIBADD=$(XLIBADD)

