lib_LTLIBRARIES= libftsclient.la
libftsclient_la_SOURCES= \
FtsAtom.cpp \
FtsSymbol.cpp \
FtsArgs.cpp \
FtsException.cpp \
FtsObject.cpp \
BinaryProtocolDecoder.cpp \
BinaryProtocolEncoder.cpp

libftsclient_la_LDFLAGS = -no-undefined

libftsclient_la_LIBADD= -lstdc++

INCLUDES=-I$(top_srcdir)/cpp/include
