packagedir=$(pkgdatadir)/packages/math/c
package_LTLIBRARIES=libmath.la
libmath_la_SOURCES= \
binop.c \
binop_fvec.c \
binop_ivec.c \
binop_number.c \
math.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/data/c/src/.libs -ldata -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libmath.so

.libs/libmath.so: $(libmath_la_OBJECTS) $(libmath_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libmath_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libmath.so
	$(INSTALL_DATA) .libs/libmath.so $(DESTDIR)$(packagedir)/libmath.so
endif

libmath_la_LDFLAGS=$(XLDFLAGS)
libmath_la_LIBADD=$(XLIBADD)

INCLUDES=-I@top_srcdir@/packages/data/c/src

noinst_HEADERS= \
binop.h \
math.h
