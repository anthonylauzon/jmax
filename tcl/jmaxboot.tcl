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
## Jmaxboot.tcl is the first tcl file to be loaded
##
## it load all the intrinsic tcl functions (i.e. the function
## jmax *need* in order to work), the jmax package system,
## and then the configuration file, that in turn should load
## the packages.
##

set jmaxRootDir [systemProperty "root"]

##
## Intrinsic functions
## Must be loaded with "source", and not sourceFile,

source $jmaxRootDir/tcl/intrinsics.tcl

##
## Intrinsic functions for the Sgi platform
##

source $jmaxRootDir/tcl/devices.tcl

##
## Tcl functions to handle projects primitives
##

source $jmaxRootDir/tcl/projects.tcl

##
## Tcl implementation of the tcl package command
##

source $jmaxRootDir/tcl/packages.tcl

##
## Max packages 
##

source $jmaxRootDir/tcl/jmaxpkg.tcl

##
## Set parameters default values
##

source $jmaxRootDir/tcl/defaults.tcl

##
## Load the system icons
##

source $jmaxRootDir/tcl/icons.tcl

##
## Define the patcher editor popup menu
## 
if {[systemProperty new] == "true"} then {
    source $jmaxRootDir/tcl/menu.tcl
}

##
## Declare the 'virtual' jmax package
##

package provide jMax 2.0.2

##
## User Configuration
##

if {[file exists [systemProperty user.home]/.jmaxrc]} then {
    source [systemProperty user.home]/.jmaxrc
}

##
## Site specific configurations
##

source $jmaxRootDir/config/site.tcl

##
## Load the machine file if existing
## 

if {[file exists $jmaxRootDir/config/$jmaxHost.tcl]} then {
    source $jmaxRootDir/config/$jmaxHost.tcl
}


##
## Process the parameters (i.e. consider defaults and command line arguments)
## 

source $jmaxRootDir/tcl/params.tcl

##
## Load the machine type file if existing
## 

if {[file exists $jmaxRootDir/config/$jmaxHostType.tcl]} then {
    source $jmaxRootDir/tcl/config/$jmaxHostType.tcl
} else {
    if {[file exists $jmaxRootDir/tcl/config/$jmaxHostType.tcl]} then {
	source $jmaxRootDir/tcl/config/$jmaxHostType.tcl
    } else {
	puts "Host Type $jmaxHostType do not exists"
	exit
    }
}

##
## do the real startup 
##

source $jmaxRootDir/tcl/startup.tcl



