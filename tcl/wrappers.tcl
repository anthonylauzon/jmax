##
## Tcl wrappers for basic kernel objects; a wrapper is the function
## used by a MaxTclInterpreter to establish an evaluation context
##

prov _MaxTclDataWrapper { this script } {
    eval $script
}

