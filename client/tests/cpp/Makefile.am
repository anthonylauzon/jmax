bin_PROGRAMS=test_standalone
test_standalone_SOURCES=test_standalone.cpp
test_standalone_LDADD=-L${top_builddir}/cpp/src/ -lftsclient -lpthread -ldl
# test_standalone_LDFLAGS=-rpath ${top_builddir}/cpp/src/.libs
# CXXFLAGS=-g -I${top_srcdir}/cpp/include
INCLUDES=-I${top_srcdir}/cpp/include

#test_standalone: $(top_srcdir)/cpp/ftsclient.lo
