##
## Tcl commands that complete the tpa format
##


proc patcher {properties body} {
    upvar 1 this this
    _patcher $this $properties $body
}


proc object {description properties} {
    upvar 1 this this
    _object $this $description $properties 
}

proc declare {description} {
    upvar 1 this this
    _declare $this $description
}

proc comment {description properties} {
    upvar 1 this this
    _comment $this $description $properties 
}

proc inlet {pos properties} {
    upvar 1 this this
    _inlet $this $pos $properties 
}

proc outlet {pos properties} {
    upvar 1 this this
    _outlet $this $pos $properties 
}






