packagedir=$(prefix)/jmax/packages/macosx/c
package_LTLIBRARIES=libmacosx.la
libmacosx_la_SOURCES= \
halaudioport.c \
macosx.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)

LINK=-$(LIBTOOL) --mode=link $(CCLD) $(AM_CFLAGS) $(CFLAGS) $(LDFLAGS) -o $@

XLDFLAGS=-no-undefined -module
XLIBADD=-L$(top_srcdir)/fts/.libs -lfts

all-local: .libs/libmacosx.so

.libs/libmacosx.so: $(libmacosx_la_OBJECTS) $(libmacosx_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libmacosx_la_OBJECTS) $(XLIBADD) -lm -lc -framework CoreAudio

install-macosx-local: .libs/libmacosx.so
	$(INSTALL_MACOSX) .libs/libmacosx.so $(DESTDIR)$(packagedir)/libmacosx.so

libmacosx_la_LDFLAGS=$(XLDFLAGS)
libmacosx_la_LIBADD=$(XLIBADD)
