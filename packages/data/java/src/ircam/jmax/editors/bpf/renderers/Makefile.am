EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
BpfBackground.java \
BpfForeground.java \
BpfRenderer.java \
PointRenderer.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/packages/data/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/data/java/src:/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
