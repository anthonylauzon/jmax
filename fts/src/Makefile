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

all_c: all
clean_c: clean

all:
	(cd sys; $(MAKE) all)
	(cd lang; $(MAKE) all)
	(cd runtime; $(MAKE) all)
	(cd tiles; $(MAKE) all)
.PHONY: all

clean:
	(cd sys; $(MAKE) clean)
	(cd lang; $(MAKE) clean)
	(cd runtime; $(MAKE) clean)
	(cd tiles; $(MAKE) clean)
.PHONY: all


tags:
	(find . \( -name "*.c" -o -name "*.h" \) -print | etags -t - )
.PHONY: tags
