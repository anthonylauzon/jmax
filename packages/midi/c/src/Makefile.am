packagedir=$(pkglibdir)/packages/midi/c
package_LTLIBRARIES=libmidi.la
libmidi_la_SOURCES= \
midi.c \
midiin.c \
midiout.c \
midiparse.c \
midiroute.c \
mtof.c \
noteoff.c \
sysex.c 

noinst_HEADERS= \
midi.h

libmidi_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
