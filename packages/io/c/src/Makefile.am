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

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libio.so

.libs/libio.so: $(libio_la_OBJECTS) $(libio_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libio_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libio.so
	$(INSTALL_DATA) .libs/libio.so $(DESTDIR)$(packagedir)/libio.so
endif

libio_la_LDFLAGS=$(XLDFLAGS)
libio_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
iomodule.h
