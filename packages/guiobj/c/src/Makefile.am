packagedir=$(pkgdatadir)/packages/guiobj/c
package_LTLIBRARIES=libguiobj.la
libguiobj_la_SOURCES= \
button.c \
comment.c \
comment_doctor.c \
console.c \
display.c \
dspcontrol.c\
errorfinder.c \
finder.c \
fork.c \
gfloat.c \
gint.c \
guiobj.c \
key.c \
keyserver.c \
messconst.c \
panel.c \
runtimeerrors.c \
scope.c \
slider.c \
toggle.c \
vecdisplay.c


INCLUDES=-I$(top_srcdir)/packages/data/c/src

noinst_HEADERS= \
guiobj.h \
keyserver.h

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/packages/data/c/src/.libs -ldata -L$(top_srcdir)/packages/utils/c/src/.libs -lutils -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libguiobj.so

.libs/libguiobj.so: $(libguiobj_la_OBJECTS) $(libguiobj_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libguiobj_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libguiobj.so
	$(INSTALL_DATA) .libs/libguiobj.so $(DESTDIR)$(packagedir)/libguiobj.so
endif

libguiobj_la_LDFLAGS=$(XLDFLAGS)
libguiobj_la_LIBADD=$(XLIBADD)

