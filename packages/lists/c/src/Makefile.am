packagedir=$(pkgdatadir)/packages/lists/c
package_LTLIBRARIES=liblists.la
liblists_la_SOURCES= \
list.c \
listarith.c \
listelement.c \
listhead.c \
listjoin.c \
listreverse.c \
lists.c \
listsize.c \
listsublist.c \
listsum.c \
listtail.c \
listcomp.c 

noinst_HEADERS= \
lists.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

endif

liblists_la_LDFLAGS=$(XLDFLAGS)
liblists_la_LIBADD=$(XLIBADD)

