# 
# jMax
# Copyright (C) 1999 by IRCAM
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
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

# To recompile, make ARCH=<architecture>
#	where architecture can be:
#		irix65r10k for IRIX6.5 R10000 processor
#		irix65r4k for IRIX6.5 R4000 processor
#		linuxpc for Linux-x86

# Change ROOTDIR according to your installation
ROOTDIR=../../../..

LIB_PROTOCOL=$(ROOTDIR)/lib/protocol/c/lib/$(ARCH)/libProtocol.a

BINS=udptest udpmessagetest
CFLAGS=-I$(ROOTDIR)/lib/protocol/c/src

all: $(BINS)

udptest: udptest.c
	$(CC) $(CFLAGS) -o udptest udptest.c $(LIB_PROTOCOL)

udpmessagetest: udpmessagetest.c
	$(CC) $(CFLAGS) -o udpmessagetest udpmessagetest.c $(LIB_PROTOCOL)

clean:
	-rm $(BINS)

