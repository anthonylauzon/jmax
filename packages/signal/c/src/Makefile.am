packagedir=$(pkgdatadir)/packages/signal/c
package_LTLIBRARIES=libsignal.la
libsignal_la_SOURCES= \
abs.c \
binop.c \
biquad.c \
bus.c \
clip.c \
delay.c \
env.c \
fir.c \
harmtap.c \
iir.c \
matrix.c \
osc.c \
osc_ieeewrap.c \
paste.c \
pink.c \
playfvec.c \
recfvec.c \
samphold.c \
samplepool.c \
signal.c \
snapshot.c \
threshold.c \
tilda.c \
wahwah.c \
wave.c \
wave_ieeewrap.c \
white.c \
zerocross.c

INCLUDES=-I$(top_srcdir)/packages/utils/c/include -I$(top_srcdir)/packages/data/c/include -I$(top_srcdir)/include

noinst_HEADERS=  \
biquad.h \
delay.h \
fir.h \
iir.h \
osc.h \
samplepool.h \
signal.h \
wave.h

libsignal_la_LDFLAGS= -module
