SUBDIRS=actions menus

packagedir=$(pkgdatadir)/packages/ispw/java/classes/ircam/jmax/editors/qlist
package_JAVA = \
FtsQListObject.java \
KeyConsumer.java \
QListFindDialog.java \
QListPanel.java \
QListUnparse.java \
QListWindow.java

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/ispw/java

