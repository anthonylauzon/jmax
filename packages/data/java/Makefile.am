SUBDIRS= ircam

packagedir=$(pkgdatadir)/packages/data/java/classes
package_JAVA= Data.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/packages/data/java
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/packages/guiobj/java/guiobj.jar:$(top_builddir)/client/java/ftsclient.jar

jardir=$(pkgdatadir)/packages/data/java
JARFILE= data.jar
jar_DATA= $(JARFILE)

JAR_DEP= \
	classpackage.stamp \
	ircam/jmax/editors/bpf/classpackage.stamp \
	ircam/jmax/editors/bpf/renderers/classpackage.stamp \
	ircam/jmax/editors/bpf/tools/classpackage.stamp \
	ircam/jmax/editors/table/classpackage.stamp \
	ircam/jmax/editors/table/menus/classpackage.stamp \
	ircam/jmax/editors/table/renderers/classpackage.stamp \
	ircam/jmax/editors/table/tools/classpackage.stamp 

CLEANFILES=$(JARFILE)

$(JARFILE): $(JAR_DEP)	
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi


install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;
