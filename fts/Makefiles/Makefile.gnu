#
#Gnu mode, force the compilation with the gcc compiler on any architecture
# with the -Wall flag set, and no other special flags; may not work in all the
# architectures
#

VENDOR := gnu
CC := gcc

ARCH_CFLAGS := -Wall -DGNU 
ARCH_LDFLAGS :=
FTS_ARCH_LIBS = $(LIBDIR)/libPortableVecLib.a

ARCH_OPT_CFLAGS := -O3
ARCH_OPT_LDFLAGS := 

FTS_SYS_LIBS = -lm 
