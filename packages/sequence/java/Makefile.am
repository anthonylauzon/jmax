SUBDIRS= src

packagedir=$(pkgdatadir)/packages/sequence/java
package_DATA = sequence.jar

JAR_DEP=$(shell find src -name "*.java")

sequence.jar: $(JAR_DEP)	
	( cd src; jar -cvf ../sequence.jar `find ircam -name "*.class"` )
