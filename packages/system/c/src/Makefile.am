packagedir=$(pkglibdir)/packages/system/c
package_LTLIBRARIES=libsystem.la
libsystem_la_SOURCES= \
sysinfo.c \
dsp1.c \
print.c \
print_tilda.c \
system.c 

noinst_HEADERS=  \
system.h

libsystem_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

INCLUDES=-I$(top_srcdir)/include

