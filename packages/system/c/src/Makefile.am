packagedir=$(pkgdatadir)/packages/system/c
package_LTLIBRARIES=libsystem.la
libsystem_la_SOURCES= \
sysinfo.c \
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

libsystem_la_LDFLAGS= -module

INCLUDES=-I$(top_srcdir)/include

