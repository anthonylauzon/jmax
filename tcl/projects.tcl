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
##  Configuration commands (for project.env and .jmaxrc)
##  


## abstractionDirectory check if in the directory given as argument
## there is a abstractions.tcl file, and load it if there; otherwise
## it add the given directory to the abstraction search path using
## the abstractionPath command
##

proc abstractionDirectory { dir } {
    if {[file exists "$dir/abstractions.tcl"]} then {
	sourceFile $dir/abstractions.tcl
    } else {
	abstractionPath $dir
    }
}

    
##
## Same thing for templates
##

proc templateDirectory { dir } {
    if {[file exists "$dir/templates.tcl"]} then {
	sourceFile $dir/templates.tcl
    } else {
	templatePath $dir
    }
}
    
proc projectDirectory {dir} {
  ucs set projectdir $dir
}

proc dataDirectory {dir} {
  global DataDirectory

  lappend DataDirectory $dir
  set path [join $DataDirectory ":"]
  ucs set defaultpath $path
}

proc jmaxSetSampleRate {rate} {
  global JmaxSampleRate

  set JmaxSampleRate $rate
  ucs set param sampling_rate $rate
}

proc jmaxGetSampleRate {} {
  global JmaxSampleRate

  return $JmaxSampleRate
}

proc jmaxSetAudioBuffer {size} {
  global JmaxAudioBuffer

  set JmaxAudioBuffer $size
  ucs set param fifo_size $size
}

proc jmaxGetAudioBuffer {} {
  global JmaxAudioBuffer

  return $JmaxAudioBuffer
}





