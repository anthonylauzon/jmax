packagedir=$(pkglibdir)/packages/guiobj/c
package_LTLIBRARIES=libguiobj.la
libguiobj_la_SOURCES= \
button.c \
comment.c \
console.c \
display.c \
dspcontrol.c\
errorfinder.c \
finder.c \
fork.c \
gfloat.c \
gint.c \
guilistener.c \
guiobj.c \
key.c \
keyserver.c \
input.c \
messconst.c \
monitor.c \
panel.c \
runtimeerrors.c \
scope.c \
slider.c \
toggle.c \
vecdisplay.c \
matdisplay.c \
xypad.c

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/packages/data/c/include -I$(top_srcdir)/packages/utils/c/include

noinst_HEADERS= \
guiobj.h \
keyserver.h

libguiobj_la_LDFLAGS= -L$(top_builddir)/fts -lfts  -module -release @JMAX_VERSION@

