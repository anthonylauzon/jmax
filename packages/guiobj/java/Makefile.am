SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/guiobj/java
JAR= guiobj.jar
package_DATA= $(JAR)
package_JAVA= Guiobj.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/guiobj/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
icons/time.stamp \
ircam/jmax/guiobj/classpackage.stamp

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class icons/*.gif ircam/jmax/guiobj/*.class
