packagedir=$(pkgdatadir)/packages/ispwmath/c
package_LTLIBRARIES=libispwmath.la
libispwmath_la_SOURCES= \
binop.c \
ispwmath.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libispwmath.so

.libs/libispwmath.so: $(libispwmath_la_OBJECTS) $(libispwmath_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libispwmath_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libispwmath.so
	$(INSTALL_DATA) .libs/libispwmath.so $(DESTDIR)$(packagedir)/libispwmath.so
endif

libispwmath_la_LDFLAGS=$(XLDFLAGS)
libispwmath_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
ispwmath.h
