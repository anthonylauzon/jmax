SUBDIRS= ircam

jardir=$(datadir)/jmax/client/java
JARFILE=ftsclient.jar
jar_DATA = $(JARFILE)

JAR_DEP=ircam/fts/client/classpackage.stamp

CLEANFILES=${JARFILE}

$(JARFILE): $(JAR_DEP)
	if test -f $(JARFILE); then \
		jar -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
	else \
		jar -cvf $(JARFILE) `find . -name "*.class"`; \
	fi

install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;
