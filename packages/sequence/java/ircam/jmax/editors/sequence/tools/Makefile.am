packagedir=$(pkglibdir)/packages/sequence/java/classes/ircam/jmax/editors/sequence/renderers
package_JAVA = \
AdderTool.java \
ArrowTool.java \
CombTool.java \
LinerTool.java \
ResizerTool.java \
SelecterTool.java \
SequenceToolManager.java \
SequenceTools.java \
VResizerTool.java \
ZoomTool.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/sequence/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/sequence/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
