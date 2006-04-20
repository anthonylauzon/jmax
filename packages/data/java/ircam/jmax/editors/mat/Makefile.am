packagedir=$(pkglibdir)/packages/data/java/classes/ircam/jmax/editors/mat
package_JAVA = \
FtsDictObject.java \
FtsMatEditorObject.java \
FtsMatObject.java \
MatEditMenu.java \
MatDataListener.java \
MatDataModel.java \
MatPanel.java \
MatRowIndex.java \
MatSimpleMenu.java \
MatTableModel.java \
MatWindow.java \
RowIndexPopupMenu.java

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/data/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/data/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
