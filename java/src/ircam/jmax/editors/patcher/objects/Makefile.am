packagedir=$(pkglibdir)/java/classes/ircam/jmax/editors/patcher/objects
package_JAVA = \
DisplayObject.java\
Editable.java\
GraphicConnection.java\
GraphicObject.java\
NumberBox.java\
ObjectInspector.java\
ObjectPopUp.java\
SensibilityArea.java\
Standard.java\
StandardControlPanel.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
