packagedir=$(pkgdatadir)/packages/math/c
package_LTLIBRARIES=libmath.la
libmath_la_SOURCES= \
binop.c \
binop_fvec.c \
binop_ivec.c \
binop_number.c \
math.c 

INCLUDES=-I@top_srcdir@/packages/data/c/src

noinst_HEADERS= \
binop.h \
math.h

libmath_la_LDFLAGS= -module

