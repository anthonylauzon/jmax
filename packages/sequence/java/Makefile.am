SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/sequence/java/classes
package_JAVA= Sequence.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/sequence/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/sequence/java
JARFILE= sequence.jar
jar_DATA= $(JARFILE)

JAR_DEP= \
	classpackage.stamp \
	ircam/jmax/editors/sequence/classpackage.stamp \
	ircam/jmax/editors/sequence/actions/classpackage.stamp \
	ircam/jmax/editors/sequence/menus/classpackage.stamp \
	ircam/jmax/editors/sequence/renderers/classpackage.stamp \
	ircam/jmax/editors/sequence/tools/classpackage.stamp \
	ircam/jmax/editors/sequence/track/classpackage.stamp 

$(JARFILE): $(JAR_DEP)	
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi


