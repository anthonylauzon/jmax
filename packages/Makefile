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
C_TARGETS=ispw guiobj lists system io sources

JAVA_TARGETS=table
MIXED_TARGETS=qlist explode

all: all_c all_java
.PHONY: all

all_c:
	for i in $(C_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all ; fi) ; done
	for i in $(MIXED_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all_c ; fi ) ; done
.PHONY: all_c

all_java:
	for i in $(JAVA_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all ; fi ) ; done
	for i in $(MIXED_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) -k all_java ; fi ) ; done
.PHONY: all_java

clean: clean_c clean_java
.PHONY: clean

clean_c:
	for i in $(C_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean ; fi ) ; done
	for i in $(MIXED_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean_c ; fi ) ; done
.PHONY: clean_c

clean_java:
	for i in $(JAVA_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean ; fi ) ; done
	for i in $(MIXED_TARGETS) ; do ( if [ -d $$i ] ; then cd $$i; $(MAKE) clean_java ; fi ) ; done
.PHONY: clean_java

