SUBDIRS= src

JARFILE = jmax.jar
pkgjavadir=$(pkgdatadir)/java
pkgjava_DATA = $(JARFILE) jacl.jar tcljava.jar

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam silk -name "*.class"` )
.PHONY: $(JARFILE)
