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

ROOTDIR=../..
include $(ROOTDIR)/VERSION

spec-files: jmax.spec buildrpm

jmax.spec: $(ROOTDIR)/VERSION
	sed -e 's/Version:.*/Version: $(JMAX_VERSION)/' jmax.spec > jmax.spec.out
	mv jmax.spec.out jmax.spec

buildrpm: $(ROOTDIR)/VERSION
	sed -e 's/JMAX_VERSION=.*/JMAX_VERSION=$(JMAX_VERSION)/' buildrpm > buildrpm.out
	mv buildrpm.out buildrpm
