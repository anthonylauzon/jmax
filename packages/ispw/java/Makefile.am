SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/ispw/java/classes
package_JAVA= Ispw.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/ispw/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/ispw/java
JARFILE = ispw.jar
jar_DATA= $(JARFILE)

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

CLEANFILES=$(JARFILE)

$(JARFILE): $(JAR_DEP)	
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi
