## Implementation of the Max pacakges
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
## jmax_pkgPath variable, that should contain a list of directories;
## packages are also looked in the jmax_sys_pkgPath variable, that is
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
    global jmax_pkgPath jmax_sys_pkgPath

    set package_loaded "no"

    if ![info exists jmax_sys_pkgPath] {
	return
    }

    if ![info exists jmax_pkgPath] {
	set jmax_pkgPath {}
    }

    for {set i [expr [llength $jmax_pkgPath] - 1]} {$i >= 0} {incr i -1} {
	set packageDir [file join [lindex $jmax_pkgPath $i] $name]
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

    for {set i [expr [llength $jmax_sys_pkgPath] - 1]} {$i >= 0} {incr i -1} {
	set packageDir [file join [lindex $jmax_sys_pkgPath $i] $name]
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


