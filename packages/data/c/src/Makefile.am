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

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/utils/c/src/.libs -lutils -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libdata.so

.libs/libdata.so: $(libdata_la_OBJECTS) $(libdata_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libdata_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libdata.so
	$(INSTALL_DATA) .libs/libdata.so $(DESTDIR)$(packagedir)/libdata.so
endif

libdata_la_LDFLAGS=$(XLDFLAGS)
libdata_la_LIBADD=$(XLIBADD)

