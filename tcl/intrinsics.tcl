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


