noinst_PROGRAMS = package
package_SOURCES = \
FtsArgs.java \
FtsAtom.java \
FtsBinaryProtocolDecoder.java \
FtsBinaryProtocolEncoder.java \
FtsClientException.java \
FtsMessageHandler.java \
FtsObject.java \
FtsPipeConnection.java \
FtsProcess.java \
FtsProtocol.java \
FtsProtocolDecoder.java \
FtsProtocolEncoder.java \
FtsServer.java \
FtsServerConnection.java \
FtsSocketConnection.java \
FtsSymbol.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

