
packagedir=$(pkglibdir)/packages/data/c
package_LTLIBRARIES=libdata.la
libdata_la_SOURCES= \
bpf.c \
data.c \
fmat.c \
fvec.c \
getrange.c \
ivec.c \
mat.c \
dict.c \
tree.c \
tabeditor.c \
dumpfile.c

INCLUDES=-I$(top_srcdir)/packages/utils/c/include -I$(top_srcdir)/packages/data/c/include -I$(top_srcdir)/include

noinst_HEADERS= \
floatfuns.h \
tabeditor.h

libdata_la_LDFLAGS= -L$(top_builddir)/fts -lfts -module -release @JMAX_VERSION@

