packagedir=$(prefix)/jmax/packages/macosx/c
package_LTLIBRARIES=libmacosx.la
libmacosx_la_SOURCES= \
halaudioport.c \
macosx.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libmacosx.so

.libs/libmacosx.so: $(libmacosx_la_OBJECTS) $(libmacosx_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libmacosx_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libmacosx.so
	$(INSTALL_DATA) .libs/libmacosx.so $(DESTDIR)$(packagedir)/libmacosx.so
endif

libmacosx_la_LDFLAGS=$(XLDFLAGS)
libmacosx_la_LIBADD=$(XLIBADD)
