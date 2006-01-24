packagedir=$(pkglibdir)/packages/data/java/classes/ircam/jmax/editors/table/actions/menus
package_JAVA = \
EditMenu.java \
TablePopupMenu.java \
TableSimpleMenu.java \
ViewMenu.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/data/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/data/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)

