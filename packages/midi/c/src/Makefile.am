packagedir=$(pkgdatadir)/packages/midi/c
package_LTLIBRARIES=libmidi.la
libmidi_la_SOURCES= \
midi.c \
midiin.c \
midiout.c \
midiport.c \
rawmidiport.c \
sysex.c 

noinst_HEADERS= \
midi.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libmidi_la_LDFLAGS=$(XLDFLAGS)
libmidi_la_LIBADD=$(XLIBADD)

