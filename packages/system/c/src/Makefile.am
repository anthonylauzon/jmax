packagedir=$(pkgdatadir)/packages/system/c
package_LTLIBRARIES=libsystem.la
libsystem_la_SOURCES= \
argument.c \
bus.c \
bus_tilda.c \
const.c \
dsp1.c \
print.c \
sigbus.c \
system.c 

noinst_HEADERS=  \
sigbus.h \
system.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libsystem_la_LDFLAGS=$(XLDFLAGS)
libsystem_la_LIBADD=$(XLIBADD)


