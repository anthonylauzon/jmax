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

ROOTDIR = ../..
include $(ROOTDIR)/Makefiles/Makefile.fts

DATE:= $(shell date | sed 's/\"//'g)
HOST:= $(shell hostname)
MODULE_CFLAGS:= -DCOMPILATION_INFO_STRING='"(compiled by $(USER) for $(ARCH) on $(DATE) on host $(HOST))"' -DCOMPILATION_ARCH_STRING='"$(ARCH)"' -DCOMPILATION_MODE_STRING='"$(MODE)"'

FTS_BIN = $(BINDIR)/fts

bin:  $(BINDIR) $(FTS_BIN)

TOCLEAN = $(FTS_BIN)

# (fd) Because of automatic rules for .a, this no longer works
# Maybe the Makefile organization is too complicated ? Why
# making archives for each subdirectory (even with 1 file) ?...
# removed include $(ROOTDIR)/Makefiles/Makefile.misc

## Rule to make an fts executable; here to simplify makefiles.
ifeq ($(ARCH),macosx)
$(FTS_BIN) : $(OBJECTS) $(FTS_LIBS) $(ARCH_LIBS) $(FTS_ARCH_LIBS)
	rm -f $(FTS_BIN)
	$(CC) $(FTS_LDFLAGS) -o $@ $(OBJDIR)/*.o $(FTS_SYS_LIBS) $(FTS_USER_LIBS_FLAGS) $(FTS_USER_LIBS)
else
$(FTS_BIN) : $(OBJECTS) $(FTS_LIB) $(ARCH_LIBS) $(FTS_ARCH_LIBS)
	rm -f $(FTS_BIN)
	$(CC) $(FTS_LDFLAGS) -o $@ $(OBJECTS) $(CC_ALL_FLAG) $(FTS_LIB) $(FTS_ARCH_LIBS) $(CC_NONE_FLAG) $(FTS_SYS_LIBS) $(FTS_USER_LIBS_FLAGS) $(FTS_USER_LIBS)
endif

