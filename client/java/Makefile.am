SUBDIRS= ircam

jardir=$(libdir)/jmax/client/java
JARFILE=ftsclient.jar
jar_DATA = $(JARFILE)

JAR_DEP=ircam/fts/client/classpackage.stamp

CLEANFILES=${JARFILE}

JAR=@JAR@
JAR_SUPPORT_UPDATE=@JAR_SUPPORT_UPDATE@

$(JARFILE): $(JAR_DEP)
	if test "$(JAR_SUPPORT_UPDATE)" = "yes"; then \
		if test -f $(JARFILE); then \
			$(JAR) -uvf $(JARFILE) `find . -name "*.class" -newer $(JARFILE)` ; \
		else \
			$(JAR) -cvf $(JARFILE) `find . -name "*.class"`; \
		fi \
	else \
		$(JAR) -cvf $(JARFILE) `find . -name "*.class"`; \
	fi

install-data-hook:
	cd $(DESTDIR)$(jardir); find . -name "*.class" -exec rm {} \;
