packagedir=$(pkgdatadir)/packages/signal/c
package_LTLIBRARIES=libsignal.la
libsignal_la_SOURCES= \
binop.c \
cut.c \
matrix.c \
osc.c \
osc_ieeewrap.c \
pink.c \
play.c \
playbpf.c \
playfvec.c \
recfvec.c \
samplepool.c \
signal.c \
tilda.c \
wave.c \
wave_ieeewrap.c \
white.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/data/c/src/.libs -ldata -L$(top_srcdir)/packages/utils/c/src/.libs -lutils -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libsignal.so

.libs/libsignal.so: $(libsignal_la_OBJECTS) $(libsignal_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libsignal_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libsignal.so
	$(INSTALL_DATA) .libs/libsignal.so $(DESTDIR)$(packagedir)/libsignal.so
endif

libsignal_la_LDFLAGS=$(XLDFLAGS)
libsignal_la_LIBADD=$(XLIBADD)

INCLUDES=-I@top_srcdir@/packages/data/c/src -I@top_srcdir@/packages/utils/c/src

noinst_HEADERS=  \
osc.h \
play.h \
samplepool.h \
signal.h \
wave.h
