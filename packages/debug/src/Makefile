#
# Makefile for the New FTS classes library
#

DISTDIR = ../../../fts
PACKAGEROOT = ..

include $(DISTDIR)/Makefiles/Makefile.lib
include Sources


all: $(LIBDIR)/libdebug.a $(LIBDIR)/libdebug.so

$(LIBDIR)/libdebug.a: $(OBJECTS)
	$(AR) $(ARFLAGS) $(LIBDIR)/libdebug.a $(OBJECTS)
	$(RANLIB) $(LIBDIR)/libdebug.a

TOCLEAN = $(LIBDIR)/libdebug.a $(LIBDIR)/libdebug.so

include $(DISTDIR)/Makefiles/Makefile.lib.end

