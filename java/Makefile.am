SUBDIRS= src

JARFILE = jmax.jar
jardir=$(prefix)/java
jar_DATA = $(JARFILE)

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam silk -name "*.class"` )
.PHONY: $(JARFILE)
