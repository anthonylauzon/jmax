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

global jmaxSampleRate jmaxAudioBuffer jmaxArch

puts "jMax default audio configuration"
puts "  sample rate: $jmaxSampleRate"
puts "  audio buffers: $jmaxAudioBuffer"
puts "  logical devices:"

ucs set sample_rate $jmaxSampleRate

if {$jmaxArch == "mips1"} {
    ucs open device out~ default as SgiALOut channels 2 fifo_size $jmaxAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $jmaxAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$jmaxArch == "irix5.3"} {
    puts "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $jmaxAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $jmaxAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$jmaxArch == "irix6.2"} {
    puts "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $jmaxAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $jmaxAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default
} elseif {$jmaxArch == "o2r5k" || $jmaxArch == "o2r10k"} {
    puts "    Analog: 2/2 (default)"
    ucs open device in~ Analog as SgiALIn ALdevice AnalogIn channels 2 fifo_size $jmaxAudioBuffer
    ucs open device out~ Analog as SgiALOut ALdevice AnalogOut channels 2 fifo_size $jmaxAudioBuffer

    ucs default in~ Analog
    ucs default out~ Analog
} elseif {$jmaxArch == "origin"} {
#    puts "Profile DACs"
#    ucs open device out~ prof_out as prof_dac channels 2 
#    ucs default out~ prof_out

    puts "    ADAT: 8/8 (default)"

# Astor config

#    ucs open device in~ ADAT as SgiALIn ALdevice ADATIn channels 8 fifo_size $jmaxAudioBuffer
    ucs open device out~ ADAT as SgiALOut ALdevice ADATOut channels 8 fifo_size $jmaxAudioBuffer
    
#    puts "    AES: 2/2"
#    ucs open device in~ AES as SgiALIn ALdevice AESIn channels 2 fifo_size $jmaxAudioBuffer
    ucs open device out~ AES as SgiALOut ALdevice AESOut channels 2 fifo_size $jmaxAudioBuffer
    
#    ucs default in~ ADAT
    ucs default out~ ADAT

#mimi config

#    post "    multiA and multiB (ADAT 1 and 2) 8/8"
#    post "    stereoA and stereoB (AES 1 and 2): 2/2"

#    ucs open device in~ multiA as SgiALIn ALdevice RAD1.ADATIn channels 8 fifo_size 512
#    ucs open device out~ multiA as SgiALOut ALdevice RAD1.ADATOut channels 8 fifo_size 512

#    ucs open device in~ stereoA as SgiALIn ALdevice RAD1.AESIn channels 2 fifo_size 512
#    ucs open device out~ stereoA as SgiALOut ALdevice RAD1.AESOut channels 2 fifo_size 512

#    ucs open device in~ multiB as SgiALIn ALdevice RAD2.ADATIn channels 8 fifo_size 512
#    ucs open device out~ multiB as SgiALOut ALdevice RAD2.ADATOut channels 8 fifo_size 512

#    ucs open device in~ stereoB as SgiALIn ALdevice RAD2.AESIn channels 2 fifo_size 512
#    ucs open device out~ stereoB as SgiALOut ALdevice RAD2.AESOut channels 2 fifo_size 512

#    ucs default in~ multiA
#    ucs default out~ multiA

} elseif {$jmaxArch == "linuxpc"} {
    puts "Profile DACs"
    ucs open device out~ prof_out as prof_dac channels 2 
    ucs default out~ prof_out
#    puts "NO DACS"

#    puts "    default: 2/2"
#    ucs open device out~ default as oss_dac
##    ucs open device in~ default as 
#   
#    ucs default out~ default 
##    ucs default in~ default
} else {

    puts "Unknow architecture, profile DACs installed"

    ucs open device out~ prof_out as prof_dac channels 2 
    ucs default out~ prof_out
}
