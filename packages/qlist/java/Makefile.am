SUBDIRS= src

packagedir=$(pkgdatadir)/packages/qlist/java
package_DATA = qlist.jar

JAR_DEP=$(shell find src -name "*.java")

qlist.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../qlist.jar `find ircam -name "*.class"` )
