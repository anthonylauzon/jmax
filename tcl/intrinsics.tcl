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
## there is a abstractions.tcl file, and load it if there, with sourceFile
## Otherwise, give an error message, asking to build the index file
##

proc abstractionDirectory { dir } {
    if {[file exists "$dir/abstractions.tcl"]} then {
	sourceFile $dir/abstractions.tcl
    } else {
	puts "No abstraction index file in $dir"
	puts "Please, build one using the jmax-make-abstraction-index shell script"
    }
}
    



##
## Commands for the .tpa format; source from a separate file
##


#source $jmaxRootDir/tcl/tpacmds.tcl



