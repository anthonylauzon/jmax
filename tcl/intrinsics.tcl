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


##
## Intrinsic functions
##


## Post, just remap to puts
## Don't use it, here for past compatibility.

proc post { args } {
    puts [ join $args]
}

## We remap the standard command "exit"
## to the jMax quit

proc exit {} {
    quit
}

##
## Supporting for the sourceFile command
## 

proc _doSourceFile {dir file} {
    _sysSourceFile $file
}


## A default implementation of the browse command
## so that the kernel code can call it
##

proc browse { location } {
    puts "No Browser installed !!!"
}


##
## The basic help command, just call up the browser 
## with a generic help html text.
##

proc help {} {
    set root [systemProperty "root"]
    browse "file:$root/doc/help.html"
}


##
## Define an host type
##

proc defineHost {name type} {
    global jmaxHostTypeTable

    set jmaxHostTypeTable($name) $type
}

##
## Load a module following the standard  naming conventions
##

proc loadModule { name libdir } {
    global jmaxArch jmaxMode

    ucs load module $name "$libdir/$jmaxArch/$jmaxMode/lib$name.so"
}
