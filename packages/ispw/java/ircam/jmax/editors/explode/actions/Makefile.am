packagedir=$(pkglibdir)/packages/ispw/java/classes/ircam/jmax/editors/explode/actions
package_JAVA = \
Actions.java \
CopyAction.java \
CutAction.java \
DuplicateAction.java \
PasteAction.java \
RedoAction.java \
SettingsAction.java \
UndoAction.java \
ViewAction.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/ispw/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
