SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/data/java
JAR= data.jar
package_DATA= $(JAR)
package_JAVA= Data.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/data/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/packages/guiobj/java/guiobj.jar:@top_srcdir@/client/java/ftsclient.jar

JAR_DEP= \
classpackage.stamp \
ircam/jmax/editors/bpf/classpackage.stamp \
ircam/jmax/editors/bpf/renderers/classpackage.stamp \
ircam/jmax/editors/bpf/tools/classpackage.stamp \
ircam/jmax/editors/table/classpackage.stamp \
ircam/jmax/editors/table/actions/classpackage.stamp \
ircam/jmax/editors/table/menus/classpackage.stamp \
ircam/jmax/editors/table/renderers/classpackage.stamp \
ircam/jmax/editors/table/tools/classpackage.stamp 


$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) *.class ircam/jmax/editors/bpf/*.class ircam/jmax/editors/bpf/renderers/*.class ircam/jmax/editors/bpf/tools/*.class ircam/jmax/editors/table/*.class ircam/jmax/editors/table/actions/*.class ircam/jmax/editors/table/menus/*.class ircam/jmax/editors/table/renderers/*.class ircam/jmax/editors/table/tools/*.class

