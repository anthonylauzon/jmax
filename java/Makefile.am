SUBDIRS= src

packagedir=$(pkgdatadir)/java
package_DATA = jacl.jar tcljava.jar jmax.jar

JAR_DEP=$(shell find src -name "*.class")

JMAX_PACKAGES=
jmax.jar: $(JAR_DEP)	
	rm jmax.jar
	( cd src; jar -cvf ../jmax.jar ircam/jmax/*.class )
	( cd src; jar -cvf ../jmax.jar ircam/jmax/*.class )
