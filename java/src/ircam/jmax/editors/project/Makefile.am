packagedir=$(pkglibdir)/java/classes/ircam/jmax/editors/project/
package_JAVA = \
ConfigPackagePanel.java\
NewProjectDialog.java\
ProjectEditor.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
