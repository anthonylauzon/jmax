SUBDIRS= icons ircam

packagedir=$(pkglibdir)/packages/ispw/java/classes
package_JAVA= Ispw.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/packages/ispw/java
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

jardir=$(pkglibdir)/packages/ispw/java
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

JAR=@JAR@
JAR_SUPPORT_UPDATE=@JAR_SUPPORT_UPDATE@

$(JARFILE): $(JAR_DEP)	
	if test "$(JAR_SUPPORT_UPDATE)" = "yes"; then \
		if test -f $(JARFILE); then \
			$(JAR) -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` `find . -name "*.gif" -newer $(JARFILE)` ; \
		else \
			$(JAR) -cvf $(JARFILE) `find . -name "*.class"` `find . -name "*.gif"` ; \
		fi \
	else \
		$(JAR) -cvf $(JARFILE) `find . -name "*.class"` `find . -name "*.gif"` ; \
	fi

install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
