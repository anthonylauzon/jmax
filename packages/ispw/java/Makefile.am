SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/ispw/java/classes
package_JAVA= Ispw.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/ispw/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/ispw/java
JAR = ispw.jar
jar_DATA= $(JAR)

JAR_DEP= \
classpackage.stamp \
icons/time.stamp \
ircam/jmax/ispw/classpackage.stamp \
ircam/jmax/editors/qlist/classpackage.stamp \
ircam/jmax/editors/qlist/actions/classpackage.stamp \
ircam/jmax/editors/qlist/menus/classpackage.stamp \
ircam/jmax/editors/explode/classpackage.stamp \
ircam/jmax/editors/explode/actions/classpackage.stamp \
ircam/jmax/editors/explode/menus/classpackage.stamp 

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class icons/*.gif ircam/jmax/ispw/*.class ircam/jmax/editors/qlist/*.class ircam/jmax/editors/qlist/actions/*.class ircam/jmax/editors/qlist/menus/*.class ircam/jmax/editors/explode/*.class ircam/jmax/editors/explode/actions/*.class ircam/jmax/editors/explode/menus/*.class 
