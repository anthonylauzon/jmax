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


##
## Simple functions to simplify Sgi configuration
##



proc defineDevice {name ucscode doc } {
    global jmaxDeviceTable
    global jmaxDeviceDoc   

    set jmaxDeviceTable($name) $ucscode
    set jmaxDeviceDoc($name)   $doc
}

proc resetAudioIn {} {
    ucs reset device in~
}
proc openAudioIn { name device {args ""}} {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio in $name: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ $name as " $jmaxDeviceTable($device) $args]
}

proc openDefaultAudioIn { device {args ""}} {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening default audio in: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ __defaultAudioIn as " $jmaxDeviceTable($device) $args]
    ucs default in~ __defaultAudioIn
}

proc openNamedDefaultAudioIn { name device {args ""} } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio in $name (also default): $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ $name as " $jmaxDeviceTable($device) $args]
    ucs default in~ $name
}

proc resetAudioOut {} {
    ucs reset device out~
}

proc openAudioOut { name device {args ""} } {
    global jmaxDeviceTable
    global jmaxDeviceDoc


    puts "Opening audio out $name: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device out~ $name as " $jmaxDeviceTable($device) $args]
}

proc openDefaultAudioOut { device {args ""} } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts $args
    puts "Opening default audio out: $jmaxDeviceDoc($device)"
    puts [concat " ucs open device out~ __defaultAudioOut as " $jmaxDeviceTable($device) $args]
    eval [concat " ucs open device out~ __defaultAudioOut as " $jmaxDeviceTable($device) $args]
    ucs default out~ __defaultAudioOut
}

proc openNamedDefaultAudioOut { name device {args ""} } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio out $name (also default): $jmaxDeviceDoc($device)"
    eval [concat " ucs open device out~ $name as " $jmaxDeviceTable($device) $args]
    ucs default out~ $name
}

proc resetMidi {} {
    ucs reset device midi
}

proc openMidi  { num device {args ""} } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening midi port $num: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device midi $num as " $jmaxDeviceTable($device) $args]
}

proc openDefaultMidi  { device {args ""} } {
   global jmaxDeviceTable
   global jmaxDeviceDoc

   puts "Opening default midi port: $jmaxDeviceDoc($device)"
   eval [concat " ucs open device midi 0 as " $jmaxDeviceTable($device) $args]
}


##
## Builtin  devices
## 

defineDevice profile {prof_dac channels 2} "Profile Dac"







