SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/sequence/java/classes
package_JAVA= Sequence.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/packages/sequence/java
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

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

CLEANFILES=$(JARFILE)

$(JARFILE): $(JAR_DEP)	
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi


install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;
