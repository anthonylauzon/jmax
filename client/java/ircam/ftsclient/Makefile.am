noinst_PROGRAMS = package
package_SOURCES = \
Fts.java \
FtsArgs.java \
FtsAtom.java \
FtsCallback.java \
FtsClientException.java \
FtsObject.java \
FtsProtocol.java \
FtsProtocolDecoder.java \
FtsServer.java


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

