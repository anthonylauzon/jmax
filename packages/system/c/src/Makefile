DISTDIR = ../../../../fts/
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libsystem.a $(LIBDIR)/libsystem.so

$(LIBDIR)/libsystem.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libsystem.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libsystem.a

TOCLEAN = $(LIBDIR)/libsystem.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

