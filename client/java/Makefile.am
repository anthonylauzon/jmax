SUBDIRS= ircam

jardir=$(datadir)/jmax/client/java
JAR=ftsclient.jar
jar_DATA = $(JAR)

JAR_DEP=ircam/fts/client/classpackage.stamp

CLEANFILES=${JAR}

$(JAR): $(JAR_DEP)
	-test -f $(JAR) || rm -f $(JAR)
	jar -cvf $(JAR) `find ircam/fts/client -name '*.class'`
