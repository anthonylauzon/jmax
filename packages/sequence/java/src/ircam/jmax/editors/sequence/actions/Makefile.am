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
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/sequence/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/sequence/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
