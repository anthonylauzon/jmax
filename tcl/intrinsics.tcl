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


## 
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
    



##
## Commands for the .tpa format; source from a separate file
##


#source $jmaxRootDir/tcl/tpacmds.tcl



