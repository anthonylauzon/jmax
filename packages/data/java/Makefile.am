SUBDIRS= src

JARFILE = data.jar
jardir=$(pkgdatadir)/packages/data/java
jar_DATA = $(JARFILE)

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam -name "*.class"` )
.PHONY: $(JARFILE)
