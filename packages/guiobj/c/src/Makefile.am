packagedir=$(pkgdatadir)/packages/guiobj/c
package_LTLIBRARIES=libguiobj.la
libguiobj_la_SOURCES= \
button.c \
comment.c \
console.c \
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
monitor.c \
panel.c \
runtimeerrors.c \
scope.c \
slider.c \
toggle.c \
vecdisplay.c \
matdisplay.c \
xypad.c
# display.c \

INCLUDES=-I$(top_srcdir)/include -I$(top_srcdir)/packages

noinst_HEADERS= \
guiobj.h \
keyserver.h

libguiobj_la_LDFLAGS= -module

