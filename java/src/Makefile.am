SUBDIRS=ircam silk

JARFILE = jmax.jar

jardir=@top_srcdir@/java
jar_DATA = $(jardir)/$(JARFILE)

clean-compile:
	rm -f $(JAR)

$(jardir)/$(JARFILE):
	jar -cvf $(jardir)/$(JARFILE) `find ircam -name "*.class"` 
.PHONY: $(JARFILE)