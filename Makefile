#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all)
	(cd packages; $(MAKE) all)

all_c:
	(cd fts; $(MAKE) all)
	(cd lib; $(MAKE) all_c)
	(cd packages; $(MAKE) all_c)

all_java:
	(cd java ; $(MAKE) all)
	(cd lib; $(MAKE) all_java)
	(cd packages; $(MAKE) all_java)

clean:
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean)
	(cd packages; $(MAKE) clean)

clean_c:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean_c)
	(cd lib; $(MAKE) clean_c)

clean_java:
	(cd java ; $(MAKE) clean)
	(cd lib; $(MAKE) clean_java)
	(cd packages; $(MAKE) clean_java)

tags:
	(find . \! \( -name '*~' \) \( -name "*.c" -o -name "*.h" -o -name "*.java" -o -name "Makefile.*" -o -name "Makefile" -o -name "*.tcl" \) -print | etags -t - )
.PHONY: tags

.PHONY: all
.PHONY: all_c
.PHONY: all_java

.PHONY: clean
.PHONY: clean_c
.PHONY: clean_java

