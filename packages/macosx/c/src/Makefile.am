packagedir=$(pkgdatadir)/packages/macosx/c
package_LTLIBRARIES=libmacosx.la
libmacosx_la_SOURCES=halaudioport.c macosxmidiport.c macosx.c qtfile.c

libmacosx_la_LDFLAGS=-module

all-local:
	cc -flat_namespace -bundle -undefined suppress -o .libs/libmacosx.so $(libmacosx_la_OBJECTS) -framework CoreAudio -framework CoreMidi -framework Carbon
