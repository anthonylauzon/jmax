packagedir=$(pkglibdir)/packages/ispw/java/classes/ircam/jmax/editors/qlist/actions
package_JAVA = \
Actions.java \
CopyAction.java \
CutAction.java \
ExportAction.java \
FindAction.java \
FindAgainAction.java \
GetAction.java \
ImportAction.java \
PasteAction.java \
SetAction.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/ispw/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
