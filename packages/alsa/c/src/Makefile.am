packagedir=$(pkgdatadir)/packages/alsa/c
package_LTLIBRARIES=libalsa.la
libalsa_la_SOURCES= \
alsa.c \
alsaaudioport.c \
alsamidi.c \
alsarawmidiport.c 

libalsa_la_CFLAGS=-DDEBUG
libalsa_la_LIBADD=-lasound

INCLUDES=-I$(top_srcdir)/include
