SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/sequence/java/classes
package_JAVA= Sequence.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/sequence/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/sequence/java
JAR = sequence.jar
package_DATA= $(JAR)

JAR_DEP= \
classpackage.stamp \
ircam/jmax/editors/sequence/classpackage.stamp \
ircam/jmax/editors/sequence/actions/classpackage.stamp \
ircam/jmax/editors/sequence/menus/classpackage.stamp \
ircam/jmax/editors/sequence/renderers/classpackage.stamp \
ircam/jmax/editors/sequence/tools/classpackage.stamp \
ircam/jmax/editors/sequence/track/classpackage.stamp 


$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class ircam/jmax/editors/sequence/*.class ircam/jmax/editors/sequence/actions/*.class ircam/jmax/editors/sequence/menus/*.class ircam/jmax/editors/sequence/renderers/*.class ircam/jmax/editors/sequence/tools/*.class ircam/jmax/editors/sequence/track/*.class

