SUBDIRS= src

JARFILE = guiobj.jar
jardir=$(prefix)/packages/guiobj/java
jar_DATA = $(JARFILE)

clean-compile:
	rm -f $(JARFILE)

$(JARFILE):
	( cd src; jar -cvf ../$(JARFILE) `find ircam -name "*.class"` )
.PHONY: $(JARFILE)
