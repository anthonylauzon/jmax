packagedir=$(pkgdatadir)/packages/data/c
package_LTLIBRARIES=libdata.la
libdata_la_SOURCES= \
bpf.c \
col.c \
copy.c \
data.c \
fill.c \
fmat.c \
fvec.c \
getinter.c \
getlist.c \
getrange.c \
getsize.c \
getval.c \
ivec.c \
mat.c \
messtab.c \
preset.c \
row.c \
value.c \
vec.c \
dumpfile.c

if MACOSX
LINK=cc -bundle -undefined error -o $@
# This should be normal use (twolevel_namespace, explicit library dependencies), but it does not work with libtool
#XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts -L$(top_srcdir)/packages/utils/c/src/.libs -lutils

# So we try flat_namespace 
#XLDFLAGS= -module
#XLIBADD= 
endif
libdata_la_LDFLAGS=$(XLDFLAGS)
libdata_la_LIBADD=$(XLIBADD)

INCLUDES=-I$(top_srcdir)/packages/utils/c/src

noinst_HEADERS= \
bpf.h \
col.h \
data.h \
fmat.h \
fvec.h \
ivec.h \
mat.h \
message.h \
messtab.h \
preset.h \
row.h \
value.h \
vec.h
