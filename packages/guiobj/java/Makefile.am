SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/guiobj/java/classes
package_JAVA= Guiobj.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/packages/guiobj/java
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/guiobj/java
JARFILE= guiobj.jar
jar_DATA= $(JARFILE)

JAR_DEP= \
	classpackage.stamp \
	icons/time.stamp \
	ircam/jmax/guiobj/classpackage.stamp

CLEANFILES=$(JARFILE)

$(JARFILE): $(JAR_DEP)	
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi

