SUBDIRS= src

packagedir=$(pkgdatadir)/java
JAR=jmax.jar
package_DATA = $(JAR)

JAR_DEP=\
	src/ircam/jmax/classpackage.stamp \
	src/ircam/jmax/dialogs/classpackage.stamp \
	src/ircam/jmax/editors/console/classpackage.stamp \
	src/ircam/jmax/editors/patcher/classpackage.stamp \
	src/ircam/jmax/editors/patcher/actions/classpackage.stamp \
	src/ircam/jmax/editors/patcher/interactions/classpackage.stamp \
	src/ircam/jmax/editors/patcher/menus/classpackage.stamp \
	src/ircam/jmax/editors/patcher/objects/classpackage.stamp \
	src/ircam/jmax/fts/classpackage.stamp \
	src/ircam/jmax/toolkit/classpackage.stamp \
	src/ircam/jmax/toolkit/actions/classpackage.stamp \
	src/ircam/jmax/toolkit/menus/classpackage.stamp \
	src/ircam/jmax/widgets/classpackage.stamp 

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	( cd src ; jar -cvf ../$(JAR) ircam/jmax/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/dialogs/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/console/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/patcher/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/patcher/actions/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/patcher/interactions/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/patcher/menus/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/editors/patcher/objects/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/fts/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/toolkit/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/toolkit/actions/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/toolkit/menus/*.class )
	( cd src ; jar -uvf ../$(JAR) ircam/jmax/widgets/*.class )
