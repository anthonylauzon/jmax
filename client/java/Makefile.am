SUBDIRS= ircam

packagedir=$(datadir)/jmax/client/java
JAR=ftsclient.jar
package_DATA = $(JAR)

JAR_DEP=ircam/ftsclient/classpackage.stamp

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) `find ircam/ftsclient -name '*.class'`
