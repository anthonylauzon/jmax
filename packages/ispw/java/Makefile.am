SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/ispw/java
JAR = ispw.jar
package_DATA= $(JAR)
package_JAVA= Ispw.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/ispw/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
icons/time.stamp \
ircam/jmax/ispw/classpackage.stamp

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class icons/*.gif ircam/jmax/ispw/*.class

