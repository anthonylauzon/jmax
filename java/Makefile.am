SUBDIRS= src

packagedir=$(pkgdatadir)/java
package_DATA = jacl.jar tcljava.jar jmax.jar

JAR_DEP=$(shell find src -name "*.java")

jmax.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../jmax.jar `find ircam silk -name "*.class"` )
