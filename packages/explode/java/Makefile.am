SUBDIRS= src

JARFILE = explode.jar
jardir=$(prefix)/packages/explode/java
jar_DATA = $(JARFILE)

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam -name "*.class"` )
.PHONY: $(JARFILE)
