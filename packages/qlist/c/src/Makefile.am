packagedir=$(pkgdatadir)/packages/qlist/c
package_LTLIBRARIES=libqlist.la
libqlist_la_SOURCES= \
qlist.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/ispw/c/src/.libs -lispw -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libqlist.so

.libs/libqlist.so: $(libqlist_la_OBJECTS) $(libqlist_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libqlist_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libqlist.so
	$(INSTALL_DATA) .libs/libqlist.so $(DESTDIR)$(packagedir)/libqlist.so
endif

libqlist_la_LDFLAGS=$(XLDFLAGS)
libqlist_la_LIBADD=$(XLIBADD)

INCLUDES=-I@top_srcdir@/packages/ispw/c/src

noinst_HEADERS= \
qlist.h
