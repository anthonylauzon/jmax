packagedir=$(pkgdatadir)/packages/numeric/c
package_LTLIBRARIES=libnumeric.la
libnumeric_la_SOURCES= \
numeric.c \
rewrap.c \
wrap.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libnumeric.so

.libs/libnumeric.so: $(libnumeric_la_OBJECTS) $(libnumeric_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libnumeric_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libnumeric.so
	$(INSTALL_DATA) .libs/libnumeric.so $(DESTDIR)$(packagedir)/libnumeric.so
endif

libnumeric_la_LDFLAGS=$(XLDFLAGS)
libnumeric_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
numeric.h
