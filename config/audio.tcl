##################################################################
#
#  FTS default audio device configuration
#
#    mips1
#    irix5.3
#    irix6.2
#    o2r5k and o2r10k
#    origin
#

post "FTS default audio configuration"
post "  sample rate: $ftsSampleRate"
post "  audio buffers: $ftsSampleRate"
post "  logical devices:"

ucs set sample_rate $ftsSampleRate

##### mips1 ######

if {$ftsArch == mips1} {
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
}

###### irix5.3 ######

elseif {$ftsArch == irix5.3} {
    post "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
}

###### irix5.3 ######

elseif {$ftsArch == irix6.2} {
    post "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
}

###### o2r5k and o2r10k ######

elseif {$ftsArch == o2r5k || $ftsArch == o2r10k} {
    post "    Analog: 2/2 (default)"
    ucs open device in~ Analog as SgiALIn ALdevice AnalogIn channels 2 fifo_size $ftsAudioBuffer
    ucs open device out~ Analog as SgiALOut ALdevice AnalogOut channels 2 fifo_size $ftsAudioBuffer

    ucs default in~ Analog
    ucs default out~ Analog
}

###### origin ######

elseif {$ftsArch == origin} {
    post "    ADAT: 8/8 (default)"
    ucs open device in~ ADAT as SgiALIn ALdevice ADATIn channels 8 fifo_size $ftsAudioBuffer
    ucs open device out~ ADAT as SgiALOut ALdevice ADATOut channels 8 fifo_size $ftsAudioBuffer
    
    post "    AES: 2/2"
    ucs open device in~ AES as SgiALIn ALdevice AESIn channels 2 fifo_size $ftsAudioBuffer
    ucs open device out~ AES as SgiALOut ALdevice AESOut channels 2 fifo_size $ftsAudioBuffer
    
    ucs default in~ ADAT
    ucs default out~ ADAT
}
