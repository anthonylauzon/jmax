packagedir=$(pkgdatadir)/java/classes/ircam/jmax/toolkit/actions
package_JAVA = \
DefaultActions.java\
EditorAction.java\
NewAction.java\
OpenAction.java\
OpenHelpSummaryAction.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@ -sourcepath $(top_srcdir)/java/src
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar
