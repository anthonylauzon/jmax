packagedir=$(pkgdatadir)/packages/io/c
package_LTLIBRARIES=libio.la
libio_la_SOURCES= \
fakestream.c \
filestream.c \
in.c \
in_out_tilda.c \
iomodule.c \
loopback.c \
out.c 

# Not compiled yet (MAC OS X)
#profileaudioport.c 
#serial.c \
#udp.c \
#wacom.c 

noinst_HEADERS= \
iomodule.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libio_la_LDFLAGS=$(XLDFLAGS)
libio_la_LIBADD=$(XLIBADD)

