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

libcontrol_la_LDFLAGS= -no-undefined -module
#libcontrol_la_LIBADD= -L$(top_srcdir)/fts/.libs -lfts

noinst_HEADERS=\
control.h
