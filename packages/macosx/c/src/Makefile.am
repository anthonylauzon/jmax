packagedir=$(prefix)/jmax/packages/macosx/c
package_DATA=libmacosx.so

OBJECTS=halaudioport.o macosx.o
LIBS= -L$(top_srcdir)/fts/.libs -lfts -lm -lc -framework CoreAudio

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)

libmacosx.so: $(OBJECTS)
	cc -bundle -undefined error -o $@ $(OBJECTS) $(LIBS)


