SUBDIRS= src

jardir=$(pkgdatadir)/java
JARFILE=jmax.jar
jar_DATA = $(JARFILE)

JAR_DEP=\
	src/jmax.jar.root \
	src/ircam/jmax/classpackage.stamp \
	src/ircam/jmax/dialogs/classpackage.stamp \
	src/ircam/jmax/editors/configuration/classpackage.stamp \
	src/ircam/jmax/editors/console/classpackage.stamp \
	src/ircam/jmax/editors/patcher/classpackage.stamp \
	src/ircam/jmax/editors/patcher/actions/classpackage.stamp \
	src/ircam/jmax/editors/patcher/interactions/classpackage.stamp \
	src/ircam/jmax/editors/patcher/menus/classpackage.stamp \
	src/ircam/jmax/editors/patcher/objects/classpackage.stamp \
	src/ircam/jmax/editors/project/classpackage.stamp \
	src/ircam/jmax/fts/classpackage.stamp \
	src/ircam/jmax/toolkit/classpackage.stamp \
	src/ircam/jmax/toolkit/actions/classpackage.stamp \
	src/ircam/jmax/toolkit/menus/classpackage.stamp \
	src/ircam/jmax/widgets/classpackage.stamp 

CLEANFILES=$(JARFILE)

JAR=@JAR@
JAR_SUPPORT_UPDATE=@JAR_SUPPORT_UPDATE@

$(JARFILE): $(JAR_DEP)
	if test "$(JAR_SUPPORT_UPDATE)" = "yes"; then \
		if test -f $(JARFILE); then \
			cd src ; $(JAR) -uvf ../$(JARFILE) jmax.jar.root `find . -name "*.class" -newer ../$(JARFILE)` ; \
		else \
			cd src ; $(JAR) -cvf ../$(JARFILE) jmax.jar.root `find . -name "*.class"`; \
		fi \
	else \
			cd src ; $(JAR) -cvf ../$(JARFILE) jmax.jar.root `find . -name "*.class"`; \
	fi



install-data-hook:
	cd $(DESTDIR)$(pkgdatadir)/java; find . -name "*.class" -exec rm {} \;
