packagedir=$(pkglibdir)/packages/control/c
package_LTLIBRARIES=libcontrol.la
libcontrol_la_SOURCES= \
bus.c \
change.c \
control.c \
delay.c \
demux.c \
dispatch.c \
fifo.c \
metro.c \
oneshot.c \
retard.c \
select.c \
speedlim.c \
stack.c \
switch.c \
sync.c \
timer.c \
tup.c

noinst_HEADERS=\
control.h

libcontrol_la_LDFLAGS= -L$(top_builddir)/fts -lfts -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include
