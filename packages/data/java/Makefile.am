SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/data/java
JAR= data.jar
package_DATA= $(JAR)
package_JAVA= Data.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/data/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
ircam/jmax/data/classpackage.stamp

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class ircam/jmax/data/*.class

