packagedir=$(pkgdatadir)/packages/sequence/java/classes/ircam/jmax/editors/sequence/actions
package_JAVA = \
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
RedoAction.java \
RemoveTrackAction.java \
UndoAction.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@ -sourcepath $(top_srcdir)/packages/sequence/java
JAVAROOT=$(top_builddir)/packages/sequence/java
CLASSPATH_ENV=CLASSPATH=$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

