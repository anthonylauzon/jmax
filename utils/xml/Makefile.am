bin_PROGRAMS=jmax2xml
jmax2xml_SOURCES=jmax2xml.c

INCLUDES=-I$(top_srcdir)/include
# jmax2xml_LDADD= $(top_srcdir)/fts/.libs/libfts.a @FTS_SYS_LIBS@
jmax2xml_LDADD= -L$(top_srcdir)/fts/@DOT_LIBS@ -lfts @FTS_SYS_LIBS@


