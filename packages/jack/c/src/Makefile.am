packagedir=$(pkgdatadir)/packages/jack/c
package_LTLIBRARIES=libjack.la
libjack_la_SOURCES= \
jack.c \
jackaudioport.c 


# libjack_la_CFLAGS = -DDEBUG `pkg-config --cflags jack`
CFLAGS = @CFLAGS@ `pkg-config --cflags jack`
libjack_la_LDFLAGS= @LDFLAGS@ `pkg-config --libs jack`

INCLUDES=-I$(top_srcdir)/include

