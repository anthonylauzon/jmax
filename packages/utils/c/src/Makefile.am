packagedir=$(pkgdatadir)/packages/utils/c
package_LTLIBRARIES=libutils.la
libutils_la_SOURCES= \
cfft.c \
complex.c \
cubic.c \
floatfuns.c \
ramp_vectors.c \
ramps.c \
random.c \
rfft_shuffle.c \
utils.c \
vec_fft.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libutils.so

.libs/libutils.so: $(libutils_la_OBJECTS) $(libutils_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libutils_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libutils.so
	$(INSTALL_DATA) .libs/libutils.so $(DESTDIR)$(packagedir)/libutils.so
endif

libutils_la_LDFLAGS=$(XLDFLAGS)
libutils_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
cfft.h \
rfft_shuffle.h \
utils.h
