##
##  Configuration commands (for project.env and .jmaxrc)
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
    
proc projectDirectory {dir} {
  ucs set projectdir $dir
}

proc dataDirectory {dir} {
  global DataDirectory

  lappend DataDirectory $dir
  set path [join $DataDirectory ":"]
  ucs set defaultpath $path
}

proc jmaxSetSampleRate {rate} {
  global JmaxSampleRate

  set JmaxSampleRate $rate
  ucs set param sampling_rate $rate
}

proc jmaxGetSampleRate {} {
  global JmaxSampleRate

  return $JmaxSampleRate
}

proc jmaxSetAudioBuffer {size} {
  global JmaxAudioBuffer

  set JmaxAudioBuffer $size
  ucs set param fifo_size $size
}

proc jmaxGetAudioBuffer {} {
  global JmaxAudioBuffer

  return $JmaxAudioBuffer
}





