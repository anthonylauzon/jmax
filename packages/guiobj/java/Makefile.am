SUBDIRS= src

packagedir=$(pkgdatadir)/packages/guiobj/java
package_DATA = guiobj.jar

JAR_DEP=$(shell find src -name "*.java")

guiobj.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../guiobj.jar `find ircam -name "*.class"` )
