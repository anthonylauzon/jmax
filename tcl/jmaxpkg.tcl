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


## Implementation of the Max packages
##
## The implementation use an extension of the standard TCL package
## mechanism, implemented by providing a special version of the
## "package unknown" function; this function extend the standard one,
## keeping complete backward compatibility with standard TCL packages
## and autoloaded function.

## A Max package named <name> is a directory named <name> containing a
## file called <name>.jpk; this file is sourced immediately, providing
## a number of local variables bounded to interesting values; the file
## should include at least the command "package provide <name>"

## This file can use a number of mechanisms to register or load various
## resources with specific tcl comnmands.

## A set of conventions keep the resources and file/directory organization
## consistents; see the doc (?? or include the doc here ??)

## Max packages (i.e. the directory itseld) are looked in the foo
## jmaxPkgPath variable, that should contain a list of directories;
## packages are also looked in the jmaxSysPkgPath variable, that is
## set from the system to the system packages.
## These variables are searched from the end, so that the 
## user can add new directory at the *end* of the list.

## If the package is not
## found, it call the tclPkgUnknown function In this release, the
## package version is ignored (but should be added ASAP)

## This function is based on the original tclPkgUnknown function
## source code

# Arguments:
# name -		Name of desired package.  Not used.
# version -		Version of desired package.  Not used.
# exact -		Either "-exact" or omitted.  Not used.


proc jmaxPkgUnknown {name version {exact {}}} {
    global jmaxPkgPath jmaxSysPkgPath

    set package_loaded "no"

    if ![info exists jmaxSysPkgPath] {
	return
    }

    if ![info exists jmaxPkgPath] {
	set jmaxPkgPath {}
    }

    for {set i [expr [llength $jmaxPkgPath] - 1]} {$i >= 0} {incr i -1} {
	set packageDir [file join [lindex $jmaxPkgPath $i] $name]
	set file [file join $packageDir $name.jpk ]

	if {[file exists $file]} then {

	    if [catch {sourceFile $file} msg] {
		# If the file is there
		# but we got an error, we return, we don't look elsewhere

		tclLog "error reading jMax package $file: $msg"
		return ""
	    } else {
		set package_loaded "yes"
	    }
        }
    }

    if {$package_loaded == "yes"} then {
	return $name
    } 

    for {set i [expr [llength $jmaxSysPkgPath] - 1]} {$i >= 0} {incr i -1} {
	set packageDir [file join [lindex $jmaxSysPkgPath $i] $name]
	set file [file join $packageDir $name.jpk ]

	if {[file exists $file]} then {
	    if [catch {sourceFile $file} msg] {
		# If the file is there
		# but we got an error, we return, we don't look elsewhere

		tclLog "error reading system jMax package $file: $msg"
		return;
	    } else {
		set package_loaded "yes"
	    }
        }
    }

    if {$package_loaded == "yes"} then {
	return $name
    }

    tclPkgUnknown $name $version
}


## set this function as the unknown function

package unknown jmaxPkgUnknown








