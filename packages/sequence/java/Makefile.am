SUBDIRS= src

packagedir=$(pkgdatadir)/packages/sequence/java/classes
JAR = sequence.jar
package_DATA= $(JAR)
package_JAVA= Sequence.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/sequence/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
src/ircam/jmax/editors/sequence/classpackage.stamp \
src/ircam/jmax/editors/sequence/actions/classpackage.stamp \
src/ircam/jmax/editors/sequence/menus/classpackage.stamp \
src/ircam/jmax/editors/sequence/renderers/classpackage.stamp \
src/ircam/jmax/editors/sequence/tools/classpackage.stamp \
src/ircam/jmax/editors/sequence/track/classpackage.stamp 


$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class src/ircam/jmax/editors/sequence/*.class src/ircam/jmax/editors/sequence/actions/*.class src/ircam/jmax/editors/sequence/menus/*.class src/ircam/jmax/editors/sequence/renderers/*.class src/ircam/jmax/editors/sequebce/tolls/*.class src/ircam/jmax/editors/sequence/track/*.class

