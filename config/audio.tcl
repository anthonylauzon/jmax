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

if {$jmaxArch == "irix6.2"} {

#    puts "Irix 6.2 audio configuration: analog 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device out~ default as SgiALOut channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device in~ default as SgiALIn channels 2 fifo_size $jmaxAudioBuffer
#    
#    ucs default out~ default 
#    ucs default in~ default

} elseif {$jmaxArch == "o2r5k" || $jmaxArch == "o2r10k"} {

#    puts "O2 audio configuration: analog 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice AnalogIn channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice AnalogOut channels 2 fifo_size $jmaxAudioBuffer

#    ucs default in~ default
#    ucs default out~ default

} elseif {$jmaxArch == "origin"} {

#    puts "Origin audio configuration: ADAT 8/8 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice ADATIn channels 8 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice ADATOut channels 8 fifo_size $jmaxAudioBuffer

#    puts "Origin audio configuration: AES 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice AESIn channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice AESOut channels 2 fifo_size $jmaxAudioBuffer

#    puts "Origin audio configuration: RAD1.ADAT 8/8 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice RAD1.ADATIn channels 8 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice RAD1.ADATOut channels 8 fifo_size $jmaxAudioBuffer

#    puts "Origin audio configuration: RAD1.AES 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice RAD1.AESIn channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice RAD1.AESOut channels 2 fifo_size $jmaxAudioBuffer

#    puts "Origin audio configuration: RAD2.ADAT 8/8 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice RAD2.ADATIn channels 8 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice RAD2.ADATOut channels 8 fifo_size $jmaxAudioBuffer

#    puts "Origin audio configuration: RAD2.AES 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device in~ default as SgiALIn ALdevice RAD2.AESIn channels 2 fifo_size $jmaxAudioBuffer
#    ucs open device out~ default as SgiALOut ALdevice RAD2.AESOut channels 2 fifo_size $jmaxAudioBuffer

#    ucs default in~ default
#    ucs default out~ default

} elseif {$jmaxArch == "linuxpc"} {

#    puts "OSS audio configuration: 2/2 @ $jmaxSampleRate Hz ($jmaxAudioBuffer)"
#    ucs open device out~ default as oss_dac
#    ucs open device in~ default as oss_adc
   
#    ucs default out~ default 
#    ucs default in~ default

} else {

    puts "unknow architecture (profile DACs installed)"

    ucs open device out~ prof_out as prof_dac channels 2

    ucs default out~ prof_out
}
