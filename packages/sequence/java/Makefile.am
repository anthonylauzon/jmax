SUBDIRS= ircam

packagedir=$(pkglibdir)/packages/sequence/java/classes
package_JAVA= Sequence.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/packages/sequence/java
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

jardir=$(pkglibdir)/packages/sequence/java
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

JAR=@JAR@
JAR_SUPPORT_UPDATE=@JAR_SUPPORT_UPDATE@

$(JARFILE): $(JAR_DEP)
	if test "$(JAR_SUPPORT_UPDATE)" = "yes"; then \
		if test -f $(JARFILE); then \
			$(JAR) -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
		else \
			$(JAR) -cvf $(JARFILE) `find . -name "*.class"`; \
		fi \
	else \
		$(JAR) -cvf $(JARFILE) `find . -name "*.class"`; \
	fi

install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
