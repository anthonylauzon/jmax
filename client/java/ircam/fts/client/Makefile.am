#noinst_JAVA= 

packagedir=$(libdir)/jmax/client/java/classes/ircam/fts/client
package_JAVA =					\
BinaryProtocolDecoder.java			\
BinaryProtocolEncoder.java			\
BinaryProtocol.java				\
FtsArgs.java					\
FtsAtom.java					\
FtsClientException.java				\
FtsMessageHandler.java				\
FtsObject.java					\
FtsPipeConnection.java				\
FtsProcess.java					\
FtsServer.java					\
FtsServerConnection.java			\
FtsSocketConnection.java			\
FtsSymbol.java 					\
RawString.java 					\
SymbolCache.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT)


# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)

