SUBDIRS= src

JARFILE = sequence.jar
jardir=$(pkgdatadir)/packages/sequence/java
jar_DATA = $(JARFILE)

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam -name "*.class"` )
.PHONY: $(JARFILE)
