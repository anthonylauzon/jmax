packagedir=$(pkgdatadir)/packages/sequence/c
package_LTLIBRARIES=libsequence.la
libsequence_la_SOURCES= \
event.c \
getdur.c \
locate.c \
module.c \
note.c \
seqmess.c \
seqfind.c \
seqmidi.c \
seqplay.c \
seqrec.c \
seqstep.c \
seqsym.c \
sequence.c \
track.c

INCLUDES=-I$(top_srcdir)/packages -I$(top_srcdir)/include

noinst_HEADERS= \
seqmess.h \
seqmidi.h \
seqsym.h

libsequence_la_LDFLAGS= -module
