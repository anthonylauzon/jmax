packagedir=$(pkgdatadir)/packages/data/java/classes/ircam/jmax/editors/bpf/renderers
package_JAVA = \
BpfBackground.java \
BpfForeground.java \
BpfRenderer.java \
PointRenderer.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@ -sourcepath $(top_srcdir)/packages/data/java
JAVAROOT=$(top_builddir)/packages/data/java
CLASSPATH_ENV=CLASSPATH=$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_builddir)/packages/data/java

