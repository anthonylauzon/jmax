EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
DisplayObject.java\
Editable.java\
GraphicConnection.java\
GraphicObject.java\
InOutlet.java\
Inlet.java\
NumberBox.java\
ObjectPopUp.java\
Outlet.java\
Patcher.java\
SensibilityArea.java\
Standard.java

JAVACX=@JAVACX@
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


