packagedir=$(pkgdatadir)/packages/signal/c
package_LTLIBRARIES=libsignal.la
libsignal_la_SOURCES= \
binop.c \
cut.c \
delay.c \
harmtap.c \
matrix.c \
osc.c \
osc_ieeewrap.c \
paste.c \
pick.c \
pink.c \
playfvec.c \
recfvec.c \
samplepool.c \
signal.c \
tilda.c \
wave.c \
wave_ieeewrap.c \
white.c

INCLUDES=-I@top_srcdir@/packages/data/c/src -I@top_srcdir@/packages/utils/c/src -I@top_srcdir@/packages/ispw/c/src

noinst_HEADERS=  \
delay.h \
osc.h \
samplepool.h \
signal.h \
wave.h

libsignal_la_LDFLAGS= -module








