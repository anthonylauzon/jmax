##
## Simple functions to simplify Sgi configuration
##



proc defineDevice {name ucscode doc } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    set jmaxDeviceTable($name) $ucscode
    set jmaxDeviceDoc($name)   $doc
}

proc resetAudioIn {} {
    ucs reset device in~
}
proc openAudioIn { name device} {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio in $name: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ $name as " $jmaxDeviceTable($device) ]
}

proc openDefaultAudioIn { device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening default audio in: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ __defaultAudioIn as " $jmaxDeviceTable($device) ]
    ucs default in~ __defaultAudioIn
}

proc openNamedDefaultAudioIn { name device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio in $name (also default): $jmaxDeviceDoc($device)"
    eval [concat " ucs open device in~ $name as " $jmaxDeviceTable($device) ]
    ucs default in~ $name
}

proc resetAudioOut {} {
    ucs reset device out~
}

proc openAudioOut { name device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc


    puts "Opening audio out $name: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device out~ $name as " $jmaxDeviceTable($device) ]
}

proc openDefaultAudioOut { device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening default audio out: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device out~ __defaultAudioOut as " $jmaxDeviceTable($device)]
    ucs default out~ __defaultAudioOut
}

proc openNamedDefaultAudioOut { name device  } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening audio out $name (also default): $jmaxDeviceDoc($device)"
    eval [concat " ucs open device out~ $name as " $jmaxDeviceTable($device) ]
    ucs default out~ $name
}

proc resetMidi {} {
    ucs reset device midi
}

proc openMidi  { num device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening midi port $num: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device midi $num as " $jmaxDeviceTable($device) ]
}

proc openDefaultMidi  { device } {
    global jmaxDeviceTable
    global jmaxDeviceDoc

    puts "Opening default midi port: $jmaxDeviceDoc($device)"
    eval [concat " ucs open device midi 0 as " jmaxDeviceTable($device) ]
}

##
## Builtin  devices
## 

defineDevice profile {prof_dac channels 2} "Profile Dac"
