packagedir=$(pkgdatadir)/packages/ispw/c
package_LTLIBRARIES=libispw.la

# The following are architecture dependant and must be determined by configure script
#osc_ieeewrap.c \
#osc_intphase.c \
#phasor_ieeewrap.c \
#phasor_intphase.c \

libispw_la_SOURCES= \
osc_ieeewrap.c \
phasor_ieeewrap.c \
Rreceive.c \
Rsend.c \
abs.c \
accum.c \
adc_tilda.c \
atomlist.c \
bag.c \
bangbang.c \
biquad.c \
cfbpass2.c \
change.c \
clip.c \
coef_h1.c \
dac_tilda.c \
delay.c \
delbuf.c \
delread.c \
deltable.c \
delwrite.c \
drunk.c \
expr_doctor.c \
exscale.c \
fft.c \
fft_ftl.c \
filters.c \
float.c \
fourpoint.c \
ftl_delay.c \
ftl_fir.c \
ftl_iir.c \
ftl_samprw.c \
ftom.c \
funbuff.c \
gate.c \
iir.c \
integer.c \
ispw.c \
ispwcomp.c \
line.c \
loadbang.c \
logscale.c \
makenote.c \
message.c \
message_doctor.c \
metro.c \
mididecrypt.c \
mtof.c \
naming.c \
nbangs.c \
noise.c \
osc.c \
pack.c \
pbank.c \
phasor.c \
pipe.c \
pitch.c \
poly.c \
prepend.c \
pt.c \
pt_meth.c \
pt_tools.c \
random.c \
resample.c \
route.c \
sampbuf.c \
samphold.c \
samppeek.c \
sampread.c \
sampunit.c \
sampwrite.c \
scale.c \
select.c \
sig1.c \
sigabs.c \
sigapass3.c \
sigarctan2.c \
sigline.c \
sigparam.c \
sigprint.c \
sigsnap.c \
sigswitch.c \
sigtable.c \
sigthres.c \
sigthrow.c \
sigthru.c \
speedlim.c \
split.c \
stripnote.c \
sustain.c \
symbol.c \
tabcycle.c \
table.c \
tabpeek.c \
tabpoke.c \
time.c \
timer.c \
trigger.c \
unpack.c \
value.c \
vd.c \
vecclip.c \
veclog.c \
vecsqrt.c \
vectrigon.c \
vexp.c \
vexp_fun.c \
vexp_if.c \
vexp_util.c \
voxalloc.c \
wahwah.c \
wavetab.c \
zerocross.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/data/c/src/.libs -ldata -L$(top_srcdir)/packages/utils/c/src/.libs -lutils -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libispw.so

.libs/libispw.so: $(libispw_la_OBJECTS) $(libispw_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libispw_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libispw.so
	$(INSTALL_DATA) .libs/libispw.so $(DESTDIR)$(packagedir)/libispw.so
endif

libispw_la_LDFLAGS=$(XLDFLAGS)
libispw_la_LIBADD=$(XLIBADD)

INCLUDES=-I@top_srcdir@/packages/data/c/src -I@top_srcdir@/packages/utils/c/src

noinst_HEADERS= \
Rsend.h \
atomlist.h \
biquad.h \
delbuf.h \
deltable.h \
fft_ftl.h \
filters.h \
fir.h \
fourpoint.h \
iir.h \
ispw.h \
naming.h \
osc.h \
phasor.h \
pt_meth.h \
pt_obj.h \
pt_tools.h \
sampbuf.h \
sampread.h \
sampunit.h \
sampwrite.h \
vd.h \
vexp.h \
vexp_util.h \
wavetab.h
