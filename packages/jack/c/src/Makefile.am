packagedir=$(pkgdatadir)/packages/jack/c
package_LTLIBRARIES=libjack.la
libjack_la_SOURCES= \
jack.c \
jackaudioport.c 


# libjack_la_CFLAGS = -DDEBUG `pkg-config --cflags jack`
CFLAGS = @CFLAGS@ @JACK_CFLAGS@
libjack_la_LDFLAGS= @LDFLAGS@ @JACK_LDFLAGS@

INCLUDES=-I$(top_srcdir)/include

