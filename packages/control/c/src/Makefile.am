packagedir=$(pkgdatadir)/packages/control/c
package_LTLIBRARIES=libcontrol.la
libcontrol_la_SOURCES= \
change.c \
control.c \
count.c \
delay.c \
demux.c \
dispatch.c \
fifo.c \
for.c \
invoke.c \
line.c \
make.c \
mess.c \
metro.c \
oneshot.c \
retard.c \
select.c \
speedlim.c \
stack.c \
switch.c \
sync.c \
tup.c

noinst_HEADERS=\
control.h

libcontrol_la_LDFLAGS=-module

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/packages/data/c/src -I$(top_srcdir)/packages/utils/c/src
