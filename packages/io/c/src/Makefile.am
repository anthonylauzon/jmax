packagedir=$(pkglibdir)/packages/io/c
package_LTLIBRARIES=libio.la
libio_la_SOURCES= \
filestream.c \
in.c \
in_out_tilda.c \
iomodule.c \
loopback.c \
out.c \
serial.c \
udp.c \
udpreceive.c \
udpsend.c 

# Not compiled yet (MAC OS X)
#profileaudioport.c 
#serial.c \
#udp.c \
#wacom.c 

noinst_HEADERS= \
iomodule.h \
jmax25_protocol.h 


libio_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
