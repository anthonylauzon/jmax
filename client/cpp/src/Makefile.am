lib_LTLIBRARIES= libftsclient.la
libftsclient_la_SOURCES= \
FtsAtom.cpp \
FtsArgs.cpp \
FtsException.cpp \
FtsObject.cpp \
FtsServerConnection.cpp \
FtsSocketConnection.cpp \
BinaryProtocolDecoder.cpp \
BinaryProtocolEncoder.cpp \
Hashtable.cpp 

libftsclient_la_LDFLAGS = -no-undefined

libftsclient_la_LIBADD= -lstdc++

INCLUDES=-I$(top_srcdir)/cpp/include

noinst_HEADERS = \
BinaryProtocolDecoder.h \
BinaryProtocolEncoder.h \
BinaryProtocol.h \
Buffer.h \
Hashtable.h \
SymbolCache.h 

bin_PROGRAMS=test1
test1_SOURCES=test1.cpp
test1_LDADD= -lftsclient -lpthread #-ldl
#test1_LDFLAGS=-rpath @TOP_DIR_PATH@/cpp/.libs
CXXFLAGS=-g -I../include
