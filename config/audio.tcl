######################################################################################
#
#  jMax default startup audio configuration
#
#  ... everything here can depend on the following system properties:

global jmaxArch 
global jmaxHost
global jmaxSampleRate 
global jmaxAudioBuffer 

#  ... aswell as any additional system property specified in the command line
# for example: jmax -myProperty "myValue"

if {[systemProperty "myProperty"] == "myValue"} { 
  # property dependent code here
}

#set jMax sample rate
ucs set sample_rate $jmaxSampleRate

######################################################################################
#
#   platform dependend startup configuration implementations
#   for the following platforms:
#
#     irix6.2
#     o2r5k and o2r10k
#     origin
#
#   The SGI command "apanel -print" gives detailed information about the
#   installed audio devices.
#

# print audio properties
puts "jMax default audio configuration"
puts "  sample rate: $jmaxSampleRate"
puts "  audio buffers: $jmaxAudioBuffer"
puts "  logical devices:"

if {$jmaxArch == "irix6.2" || $jmaxArch == "irix5.3" || $jmaxArch == "mips1"} {

    puts "    default: 2/2"
    ucs open device out~ default as SgiALOut channels 2 fifo_size $jmaxAudioBuffer
    ucs open device in~ default as SgiALIn channels 2 fifo_size $jmaxAudioBuffer
    
    ucs default out~ default 
    ucs default in~ default

} elseif {$jmaxArch == "o2r5k" || $jmaxArch == "o2r10k"} {

    puts "    analog: 2/2 (default)"
    ucs open device in~ analog as SgiALIn ALdevice AnalogIn channels 2 fifo_size $jmaxAudioBuffer
    ucs open device out~ analog as SgiALOut ALdevice AnalogOut channels 2 fifo_size $jmaxAudioBuffer

    ucs default in~ analog
    ucs default out~ analog

} elseif {$jmaxArch == "origin"} {

    # one ADAT card

    #puts "    multi: 8/8 (default)"
    #ucs open device in~ multi as SgiALIn ALdevice ADATIn channels 8 fifo_size $jmaxAudioBuffer
    #ucs open device out~ multi as SgiALOut ALdevice ADATOut channels 8 fifo_size $jmaxAudioBuffer

#puts "    stereo: 2/2"
#ucs open device in~ stereo as SgiALIn ALdevice AESIn channels 2 fifo_size $jmaxAudioBuffer
#ucs open device out~ stereo as SgiALOut ALdevice AESOut channels 2 fifo_size $jmaxAudioBuffer

#ucs default in~ multi
#ucs default out~ multi

    # two ADAT cards

#     puts "    multi1: 8/8 (default)"
#     ucs open device in~ multi1 as SgiALIn ALdevice RAD1.ADATIn channels 8 fifo_size $jmaxAudioBuffer
#     ucs open device out~ multi1 as SgiALOut ALdevice RAD1.ADATOut channels 8 fifo_size $jmaxAudioBuffer

#    puts "    stereo1: 2/2"
#    ucs open device in~ stereo1 as SgiALIn ALdevice RAD1.AESIn channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device out~ stereo1 as SgiALOut ALdevice RAD1.AESOut channels 2 fifo_size $jmaxAudioBuffer

    #puts "    multi2: 8/8"
    #ucs open device in~ multi2 as SgiALIn ALdevice RAD2.ADATIn channels 8 fifo_size $jmaxAudioBuffer
    #ucs open device out~ multi2 as SgiALOut ALdevice RAD2.ADATOut channels 8 fifo_size $jmaxAudioBuffer

#    puts "    stereo2: 0/2"
    #ucs open device in~ stereo2 as SgiALIn ALdevice RAD2.AESIn channels 2 fifo_size $jmaxAudioBuffer
    ucs open device out~ stereo2 as SgiALOut ALdevice RAD2.AESOut channels 2 fifo_size $jmaxAudioBuffer

#     ucs default in~ multi1
#     ucs default out~ multi1
#    ucs default in~ stereo1
#    ucs default out~ stereo1

} elseif {$jmaxArch == "linuxpc"} {

    puts "    default: 2/2"
#    ucs open device out~ default as oss_dac
    ucs open device out~ default as oss_dac fragment_size 4096 max_fragments 8
    #ucs open device in~ default as oss_adc
   
    ucs default out~ default 
    #ucs default in~ default

} else {

    puts "unknow architecture (profile DACs installed)"

    ucs open device out~ prof_out as prof_dac channels 2

    ucs default out~ prof_out
}
