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


#
#  jMax site configuration
#  
#  This file is executed at start up; you can add tcl code
#  here to simplify the user configuration in your site
#
#  For example, for each type of machines you can write a tcl
#  function that set the standard configuration for your site,
#  and your users can just execute this function in their .jmaxrc
#  
#  Like:
#
#  proc octaneStandardConfig {} {
# 	openDefaultMidi     "SerialPort2"
#	openDefaultAudioIn  ADATIn   8 
#	openDefaultAudioOut ADATOut  8     
#  }
#
#
# Also, in order to simplify your users .jmaxrc file, you can declare the host type
# of your hosts by using the function defineHostType <host> <type>

defineHost "astor.ircam.fr"      origin
defineHost "stradivari.ircam.fr" octane
defineHost "maggini.ircam.fr"    octane
defineHost "amati.ircam.fr"      octane
defineHost "guarneri.ircam.fr"   octane
defineHost "marimba.ircam.fr"    o2r5ki63
defineHost "libido.ircam.fr"     linuxpc
defineHost "linotte.ircam.fr"    linuxpc
defineHost "lindberg.ircam.fr"   linuxpc

