SUBDIRS= src lib

packagedir=$(pkgdatadir)/java
package_DATA = jmax.jar

JAR_DEP=$(shell find src -name "*.class")

jmax.jar: $(JAR_DEP)	
	-rm jmax.jar
	( cd src ; jar -cvf ../jmax.jar ircam/jmax/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/dialogs/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/console/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/patcher/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/patcher/actions/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/patcher/interactions/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/patcher/menus/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/editors/patcher/objects/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/fts/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/mda/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/pkg/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/scm/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/scm/kawa/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/scm/silk/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/script/tcl/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/toolkit/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/toolkit/actions/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/toolkit/menus/*.class )
	( cd src ; jar -uvf ../jmax.jar ircam/jmax/widgets/*.class )
	( cd src ; jar -uvf ../jmax.jar silk/*.class )
