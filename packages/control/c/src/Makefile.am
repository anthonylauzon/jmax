packagedir=$(pkgdatadir)/packages/control/c
package_LTLIBRARIES=libcontrol.la
libcontrol_la_SOURCES= \
control.c \
count.c \
demux.c \
fit.c \
for.c \
oneshot.c \
range.c \
switch.c \
sync.c 

noinst_HEADERS=\
control.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

libcontrol_la_LDFLAGS=$(XLDFLAGS)
libcontrol_la_LIBADD=$(XLIBADD)

