packagedir=$(pkgdatadir)/java/classes/ircam/jmax/toolkit/menus
package_JAVA = \
DefaultFileMenu.java    \
DefaultHelpMenu.java    \
DefaultProjectMenu.java \
EditorMenu.java         \
MaxWindowJMenu.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@ -sourcepath $(top_srcdir)/java/src
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar

