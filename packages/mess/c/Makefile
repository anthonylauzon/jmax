#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
all:
	(cd src; $(MAKE) -k all)
.PHONY: all

clean:
	(cd src; $(MAKE) clean)
.PHONY: clean

install:
	$(INSTALL_DIR) $(lib_install_dir)/packages/mess/c/lib/$(ARCH)/opt
	( cd lib/$(ARCH)/opt ; $(INSTALL_LIB) libmess.so $(lib_install_dir)/packages/mess/c/lib/$(ARCH)/opt )
.PHONY: install


