EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
BeanPanel.java\
ColorPropertyEditor.java\
ConsoleArea.java\
IncrementController.java\
IncrementListener.java\
IndicatorWithMemory.java\
Led.java\
MemoryLed.java\
ObjectSelectedListener.java\
ObjectSetViewer.java\
SimpleLed.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAC=@JAVAC@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES= .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~


