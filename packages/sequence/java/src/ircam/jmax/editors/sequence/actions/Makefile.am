SUBDIRS=

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
Actions.java \
CopyAction.java \
CutAction.java \
DuplicateAction.java \
ExportAction.java \
ImportAction.java \
MergeAction.java \
MoveClosedTrackToAction.java \
MoveMidiTrackToAction.java \
MoveMonoTrackToAction.java \
PasteAction.java \
PrintAction.java \
RedoAction.java \
RemoveTrackAction.java \
UndoAction.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/packages/sequence/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/sequence/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
