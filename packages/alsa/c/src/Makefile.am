packagedir=$(pkglibdir)/packages/alsa/c
package_LTLIBRARIES=libalsa.la
libalsa_la_SOURCES= \
alsa.c \
alsaaudiomanager.c \
alsaaudioport.c \
alsamidi.c \
alsarawmidiport.c \
alsaseqmidiport.c \
alsaseqmidi.c  

libalsa_la_CFLAGS=@ALSA_CFLAGS@
libalsa_la_LIBADD=-L$(top_builddir)/fts -lfts @ALSA_LIBS@

INCLUDES=-I$(top_srcdir)/include

noinst_HEADERS= alsamidi.h \
		alsaseqmidi.h \
		alsaaudio.h 

