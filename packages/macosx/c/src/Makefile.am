packagedir=$(pkgdatadir)/packages/macosx/c
package_LTLIBRARIES=libmacosx.la
libmacosx_la_SOURCES=halaudioport.c macosx.c

libmacosx_la_LDFLAGS=-no-undefined -module
XLIBADD=-L$(top_srcdir)/fts/.libs -lfts -lm -lc
libmacosx_la_LIBADD=$(XLIBADD) -lCoreAudio 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)

all-local: .libs/libmacosx.so

.libs/libmacosx.so: $(libmacosx_la_OBJECTS) $(libmacosx_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libmacosx_la_OBJECTS) $(XLIBADD) -framework CoreAudio

install-data-local: .libs/libmacosx.so
	$(INSTALL_DATA) .libs/libmacosx.so $(DESTDIR)$(packagedir)/libmacosx.so
