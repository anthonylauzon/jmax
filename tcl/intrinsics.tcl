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
