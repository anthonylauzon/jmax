SUBDIRS= src

packagedir=$(pkgdatadir)/packages/data/java
package_DATA = data.jar

JAR_DEP=$(shell find src -name "*.java")

data.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../data.jar `find ircam -name "*.class"` )
