packagedir=$(pkgdatadir)/packages/control/c
package_LTLIBRARIES=libcontrol.la
libcontrol_la_SOURCES= \
control.c \
count.c \
demux.c \
dispatch.c \
fifo.c \
for.c \
invoke.c \
make.c \
mess.c \
oneshot.c \
range.c \
stack.c \
switch.c \
sync.c \
tup.c

noinst_HEADERS=\
control.h

libcontrol_la_LDFLAGS=-module

INCLUDES=-I$(top_srcdir)/include
