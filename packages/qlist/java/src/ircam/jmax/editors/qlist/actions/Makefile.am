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
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/qlist/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/qlist/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
