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

global ftsSampleRate ftsAudioBuffer ftsArch

puts "FTS default audio configuration"
puts "  sample rate: $ftsSampleRate"
puts "  audio buffers: $ftsAudioBuffer"
puts "  logical devices:"

ucs set sample_rate $ftsSampleRate

if {$ftsArch == "mips1"} {
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$ftsArch == "irix5.3"} {
    puts "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$ftsArch == "irix6.2"} {
    puts "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $ftsAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $ftsAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$ftsArch == "o2r5k" || $ftsArch == "o2r10k"} {
    puts "    Analog: 2/2 (default)"
    ucs open device in~ Analog as SgiALIn ALdevice AnalogIn channels 2 fifo_size $ftsAudioBuffer
    ucs open device out~ Analog as SgiALOut ALdevice AnalogOut channels 2 fifo_size $ftsAudioBuffer

    ucs default in~ Analog
    ucs default out~ Analog
} elseif {$ftsArch == "origin"} {
    puts "    ADAT: 8/8 (default)"
#    ucs open device in~ ADAT as SgiALIn ALdevice ADATIn channels 8 fifo_size $ftsAudioBuffer
    ucs open device out~ ADAT as SgiALOut ALdevice ADATOut channels 8 fifo_size $ftsAudioBuffer
    
    puts "    AES: 2/2"
#    ucs open device in~ AES as SgiALIn ALdevice AESIn channels 2 fifo_size $ftsAudioBuffer
    ucs open device out~ AES as SgiALOut ALdevice AESOut channels 2 fifo_size $ftsAudioBuffer
    
    ucs default in~ ADAT
    ucs default out~ ADAT
}
