SUBDIRS= ircam

packagedir=$(pkgdatadir)/java
JAR=ftsclient.jar
package_DATA = ${JAR}

clean-compile:
	rm -f ${JAR}

${JAR}:
	-rm ${JAR}
	jar -cvf ${JAR} ircam/ftsclient/*.class
