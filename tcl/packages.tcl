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


# TCL simple and naive Implementation of the package command.
# it just completely ignore the version thing, and provide
# minimal functionalities, so that we can immediately build
# the Max packages on the Tcl packages, waiting for the Jacl
# implementation of packages (the unknown handler is there already).
#
# Only the following subcommands are implemented:
#
#         package forget package
#         package ifneeded package version ?script? 
#         package provide package ?version? 
#         package require ?-exact? package ?version? 
#         package unknown ?command? 
#
# The others are simply ignored.
#
# The package module use two arrays, package_loaded, and package_ifneeded
# and a global variable

set packageUnknownCommand ""


proc package {cmd {arg1 "void"} {arg2 "void"} {arg3 void}} {
    switch $cmd {
	forget    {packageForget $arg1}
	ifneeded  {packageIfNeeded $arg1 $arg3}
	provide   {packageProvide $arg1}
	require   {if {$arg1 == "-exact"} then {packageRequire $arg2 $arg3} else {packageRequire $arg1 $arg2}}
	names     {packageNames}
	unknown   {packageUnknown $arg1}
    }
}

##   packageForget
##   delete the entries relative to the package in the 
##   tables

proc packageForget {package} {
    global package_loaded package_ifneeded

    if [info exists package_loaded($package)] then {
	unset package_loaded($package)
    }

    if [info exists package_ifneeded($package)] then {
	unset package_ifneeded($package)
    }
}

##
##  packageIfNeeded
##  store the script in a global table
##

proc packageIfNeeded {package script} {
    global package_ifneeded

    set package_ifneeded($package) $script

    return ""
}


##  package Provide
##  Register the fact the package has been loaded in the package_loaded 
##  global arrays

proc packageProvide {package} {
    global package_loaded

    set package_loaded($package) "yes"

    return package
}
    

##
## packageRequire
## 
## the real engine; first, look if the package has been loaded;
## if not, look in the ifneeded table, and if a script is existing,
## execute it in the global space (upleve #0 ???)
## if the script is not found, call the packageUnknownCommand in the global
## space


proc packageRequire {package version} {
    global package_loaded package_ifneeded packageUnknownCommand

#return if the package is there

    if [info exists package_loaded($package)] then {
	return $package
    }

#execute the ifneeded script if there

    if [info exists package_ifneeded($package)] then {
	uplevel #0 $package_ifneeded($package)
    }

#return if the package is there

    if [info exists package_loaded($package)] then {
	return $package
    }

## do the unknown command

    if [info exists packageUnknownCommand] then {
	uplevel #0 $packageUnknownCommand $package $version
    }


#return if the package is there

    if [info exists package_loaded($package)]  then {
	return $package
    }

## check again if the unknown command specified an ifneeded thing

    if [info exists package_ifneeded($package)] then {
	uplevel #0 $package_ifneeded($package)
    } 

    if [info exists package_loaded($package)] then {
	return $package
    }

    puts "package $package not found"
    return ""
}

##
## Package Names; extract the list of known/loaded packages
##

proc packageNames {} {
    global package_loaded package_ifneeded

    if [array exists package_loaded] {
	set list [array names package_loaded]
    } else {
	set list {}
    }

    if [array exists package_ifneeded] {
	set listIf [array names package_ifneeded]

	foreach package $listIf {
	    if {[lsearch -exact $list $package] < 0} {
		lappend list $package
	    }
	}
    }

    return $list
}

##
## Package Unknown : just store the script in the packageUnknownCommand
## global variable
##

proc packageUnknown {cmd} {
    global packageUnknownCommand

    if {$cmd != ""} then {
	set packageUnknownCommand $cmd
    } else {
	unset packageUnknownCommand
    }

    return ""
}

##
## This tcl function *have* to be called *after* user initialization
##  (after the loading of .ermesrc) to set the pacakge related variables
## Actually, redo a part of the init.tcl code, beacause we don't have (yet ?)
## access in Jacl to the shell environemnt (the $env variable do not work),
## so some init time work must be done after loading the user code.

proc packageInit { } {
    global auto_path env tcl_pkgPath

# Compute the auto path to use in this interpreter.
# (auto_path could be already set, in safe interps for instance)

    if {![info exists auto_path]} {
	if [catch {set auto_path $env(TCLLIBPATH)}] {
	    set auto_path ""
	}
    }

    if {[lsearch -exact $auto_path [info library]] < 0} {
	lappend auto_path [info library]
    }

    catch {
	foreach dir $tcl_pkgPath {
	    if {[lsearch -exact $auto_path $dir] < 0} {
		lappend auto_path $dir
	    }
	}
    }

    package unknown tclPkgUnknown
}


##
## We assume here that the user code has been already executed, and
## packages.tcl is loaded from the system ermes.tcl file.
##

packageInit



