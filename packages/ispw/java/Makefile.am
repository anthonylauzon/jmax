SUBDIRS= src

packagedir=$(pkgdatadir)/packages/ispw/java
package_DATA = ispw.jar

JAR_DEP=$(shell find src -name "*.java")

ispw.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../ispw.jar `find ircam -name "*.class"` )
