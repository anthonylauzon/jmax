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

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/liblists.so

.libs/liblists.so: $(liblists_la_OBJECTS) $(liblists_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(liblists_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/liblists.so
	$(INSTALL_DATA) .libs/liblists.so $(DESTDIR)$(packagedir)/liblists.so
endif

liblists_la_LDFLAGS=$(XLDFLAGS)
liblists_la_LIBADD=$(XLIBADD)

noinst_HEADERS= \
lists.h
