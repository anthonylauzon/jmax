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
## The config command
##

source $jmaxRootDir/tcl/config.tcl

##
## Wrapper for MDA and TclMDA
## 

source $jmaxRootDir/tcl/wrappers.tcl

##
## Tcl implementation of the tcl package command
##

sourceFile $jmaxRootDir/tcl/packages.tcl

##
## Max packages 
##

sourceFile $jmaxRootDir/tcl/jmaxpkg.tcl

##
## Declare the 'virtual' jmax package
##

package provide jMax 2.0.2


##
## User Configuration (use source to get global variables)
##

if {[file exists [systemProperty user.home]/.jmaxrc]} then {
    source [systemProperty user.home]/.jmaxrc
}

##
## startup with configuration
##

sourceFile $jmaxRootDir/tcl/startup.tcl
