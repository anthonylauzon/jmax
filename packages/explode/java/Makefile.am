SUBDIRS= src

packagedir=$(pkgdatadir)/packages/explode/java
package_DATA = explode.jar

JAR_DEP=$(shell find src -name "*.java")

explode.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../explode.jar `find ircam -name "*.class"` )
