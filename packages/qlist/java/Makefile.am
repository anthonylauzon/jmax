SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/qlist/java
JAR= qlist.jar
package_DATA= $(JAR)
package_JAVA= Qlist.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/qlist/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
ircam/jmax/editors/qlist/classpackage.stamp \
ircam/jmax/editors/qlist/actions/classpackage.stamp \
ircam/jmax/editors/qlist/menus/classpackage.stamp 

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class ircam/jmax/editors/qlist/*.class ircam/jmax/editors/qlist/actions/*.class ircam/jmax/editors/qlist/menus/*.class
