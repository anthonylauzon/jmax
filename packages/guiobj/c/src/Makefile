DISTDIR = ../../../../fts/
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources

all: $(LIBDIR)/libguiobj.a $(LIBDIR)/libguiobj.so

$(LIBDIR)/libguiobj.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libguiobj.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libguiobj.a

TOCLEAN = $(LIBDIR)/libguiobj.a

include $(DISTDIR)/Makefiles/Makefile.lib.end

