packagedir=$(pkgdatadir)/packages/system/c
package_LTLIBRARIES=libsystem.la
libsystem_la_SOURCES= \
argument.c \
bus.c \
bus_tilda.c \
const.c \
dsp1.c \
print.c \
sigbus.c \
system.c \
ucsobj.c 

# Mac OS X libtool/twolevel_namespace mess (see ChangeLog)
if MACOSX

# This causes libtool to issue a warning and not to produce the .so 
XLDFLAGS= -no-undefined -module
XLIBADD= -L$(top_srcdir)/fts/.libs -lfts 

# So we add this explicitely !!!
all-local: .libs/libsystem.so

.libs/libsystem.so: $(libsystem_la_OBJECTS) $(libsystem_la_DEPENDENCIES)
	cc -bundle -undefined error -o $@ $(libsystem_la_OBJECTS) $(XLIBADD) -lm -lc 

install-data-local: .libs/libsystem.so
	$(INSTALL_DATA) .libs/libsystem.so $(DESTDIR)$(packagedir)/libsystem.so
endif

libsystem_la_LDFLAGS=$(XLDFLAGS)
libsystem_la_LIBADD=$(XLIBADD)

noinst_HEADERS=  \
sigbus.h \
system.h
