SUBDIRS=

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
Actions.java \
CopyAction.java \
CutAction.java \
ExportAction.java \
FindAction.java \
FindAgainAction.java \
GetAction.java \
ImportAction.java \
PasteAction.java \
PrintAction.java \
SetAction.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/packages/qlist/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/qlist/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
