# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

JMAXDISTDIR= ../../..
PACKAGEROOT = ..
PACKAGELIB=$(LIBDIR)/libunixdtd.so

##
## Extra definitions
##

# Note: some $(SERVER_OBJECTS) are not in the SOURCES because they are not added to the .so library
#SERVER_OBJECTS=$(OBJDIR)/dtdmain.o $(OBJDIR)/dtdfifo.o
SERVER_OBJECTS=$(OBJDIR)/dtdmain.o $(OBJDIR)/dtdfifo.o $(OBJDIR)/libtime.o # This is for debug
DTD_BIN = $(BINDIR)/dtd
FTS_LIB=$(JMAXDISTDIR)/fts/lib/$(ARCH)/$(MODE)/libfts.a
TOCLEAN=$(SERVER_OBJECTS) $(PACKAGE_LIB) $(DTD_BIN)  

##
## Standard Makefile for packages
##
include $(JMAXDISTDIR)/Makefiles/Makefile.package

##
## Extra rules
##

all: $(LIBDIR) $(OBJDIR) $(BINDIR) $(PACKAGELIB) $(DTD_BIN)

$(DTD_BIN): $(SERVER_OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^ $(FTS_LIB) $(FTS_SYS_LIBS)

$(BINDIR):
	mkdir -p $(BINDIR)

