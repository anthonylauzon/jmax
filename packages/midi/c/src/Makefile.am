packagedir=$(pkgdatadir)/packages/midi/c
package_LTLIBRARIES=libmidi.la
libmidi_la_SOURCES= \
midi.c \
midiin.c \
midiout.c \
midiparse.c \
mtof.c \
noteoff.c \
sysex.c 

noinst_HEADERS= \
midi.h

libmidi_la_LDFLAGS= -module

INCLUDES=-I$(top_srcdir)/include
