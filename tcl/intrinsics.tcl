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

