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
	(cd java ; $(MAKE) all)
	(cd c ; $(MAKE) all)
.PHONY: all

clean:
	(cd java ; $(MAKE) clean)
	(cd c ; $(MAKE) clean)
.PHONY: clean

all_c:
	(cd c ; $(MAKE) all)
.PHONY: all

clean_c:
	(cd c ; $(MAKE) clean)
.PHONY: clean

all_java:
	(cd java ; $(MAKE) all)
.PHONY: all

clean_java:
	(cd java ; $(MAKE) clean)
.PHONY: clean
