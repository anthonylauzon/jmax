##
## Simple functions to simplify Sgi configuration
##

proc resetAudioIn {} {
    ucs reset device in~
}
proc openAudioIn { name device ch } {
    ucs open device in~ $name as SgiALIn ALdevice $device channels $ch
}

proc openDefaultAudioIn { device ch } {
    ucs open device in~ defaultAudioIn as SgiALIn ALdevice $device channels $ch
    ucs default in~ defaultAudioIn
}

proc openNamedDefaultAudioIn { name device ch } {
    ucs open device in~ $name as SgiALIn ALdevice $device channels $ch
    ucs default in~ $name
}

proc resetAudioOut {} {
    ucs reset device out~
}

proc openAudioOut { name device ch } {
    ucs open device out~ $name as SgiALOut ALdevice $device channels $ch
}

proc openDefaultAudioOut { device ch } {
    ucs open device out~ defaultAudioOut as SgiALOut ALdevice $device channels $ch
    ucs default out~ defaultAudioOut
}

proc openNamedDefaultAudioOut { name device ch } {
    ucs open device out~ $name as SgiALOut ALdevice $device channels $ch
    ucs default out~ $name
}

proc resetMidi {} {
    ucs reset device midi
}

proc openMidi  { num port } {
    ucs open device midi $num as sgi_midi port $port
}

proc openDefaultMidi  { port } {
    ucs open device midi 0 as sgi_midi port $port
}
