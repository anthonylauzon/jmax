SUBDIRS= icons ircam

packagedir=$(pkgdatadir)/packages/guiobj/java/classes
package_JAVA= Guiobj.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/packages/guiobj/java
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

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
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` `find . -name "*.gif" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"` `find . -name "*.gif"` ; \
	fi


install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
