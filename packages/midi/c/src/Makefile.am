packagedir=$(pkgdatadir)/packages/midi/c
package_LTLIBRARIES=libmidi.la
libmidi_la_SOURCES= \
midi.c \
midiin.c \
midiout.c \
midiport.c \
rawmidiport.c \
sysex.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libmidi.so

.libs/libmidi.so: $(libmidi_la_OBJECTS) $(libmidi_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libmidi_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libmidi.so
	$(INSTALL_DATA) .libs/libmidi.so $(DESTDIR)$(packagedir)/libmidi.so
endif

libmidi_la_LDFLAGS=$(XLDFLAGS)
libmidi_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
midi.h

