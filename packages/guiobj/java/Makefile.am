SUBDIRS= src

packagedir=$(pkgdatadir)/packages/guiobj/java
JAR= guiobj.jar
package_DATA = $(JAR)

JAR_DEP=src/ircam/jmax/guiobj/classpackage.stamp

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)	
	-test -f $(JAR) || rm -f $(JAR)
	( cd src; jar -cvf ../$(JAR) `find ircam -name "*.class"` )
