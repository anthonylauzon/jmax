packagedir=$(pkgdatadir)/packages/control/c
package_LTLIBRARIES=libcontrol.la
libcontrol_la_SOURCES= \
control.c \
count.c \
demux.c \
fit.c \
for.c \
oneshot.c \
range.c \
switch.c \
sync.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libcontrol.so

.libs/libcontrol.so: $(libcontrol_la_OBJECTS) $(libcontrol_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libcontrol_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libcontrol.so
	$(INSTALL_DATA) .libs/libcontrol.so $(DESTDIR)$(packagedir)/libcontrol.so
endif

libcontrol_la_LDFLAGS=$(XLDFLAGS)
libcontrol_la_LIBADD=$(XLIBADD)

noinst_HEADERS=\
control.h
