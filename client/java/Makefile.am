SUBDIRS= ircam

packagedir=$(datadir)/jmax/client/java
JAR=ftsclient.jar
package_DATA = $(JAR)

JAR_DEP=$(shell find ircam -name "*.class")

clean-compile:
	rm -f ${JAR}

$(JAR): $(JAR_DEP)
	-rm $(JAR)
	jar -cvf $(JAR) `find ircam -name '*.class'`
