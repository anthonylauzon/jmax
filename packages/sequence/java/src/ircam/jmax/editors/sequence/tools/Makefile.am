SUBDIRS=

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
AdderTool.java \
ArrowTool.java \
CombTool.java \
DeleteTool.java \
LinerTool.java \
ResizerTool.java \
ScrollerTool.java \
SelecterTool.java \
SequenceTools.java \
VResizerTool.java \
ZoomTool.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
