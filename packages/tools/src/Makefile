MODULE_CFLAGS= 

DISTDIR = ../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libtools.a $(LIBDIR)/libtools.so

$(LIBDIR)/libtools.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libtools.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libtools.a

TOCLEAN = $(LIBDIR)/libtools.a $(LIBDIR)/libtools.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

