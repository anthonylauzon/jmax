packagedir=$(pkgdatadir)/packages/ispw/java/classes/ircam/jmax/editors/qlist/menus
package_JAVA = \
EditMenu.java \
FileMenu.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@ 
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/ispw/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
