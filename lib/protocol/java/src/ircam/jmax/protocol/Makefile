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
#

ROOTDIR= ../../../..
PACKAGE_DIR=ircam/jmax/protocol

CLASSES=$(shell ls *.java | sed s/.java/.class/)

CLASSPATH=$(ROOTDIR)/src:$(ROOTDIR)/classes

JAVAC=javac
JFLAGS= -d $(ROOTDIR)/classes

all: $(CLASSES)

%.class: %.java
	( CLASSPATH=$(CLASSPATH) ; export CLASSPATH ; $(JAVAC) $(JFLAGS) $< )


