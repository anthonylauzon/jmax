######################################################################################
#
#  jMax default startup audio configuration
#
#  ... everything here can depend on the following system properties:

global jmaxArch 
global jmaxHost

#  ... aswell as any additional system property specified in the command line
# for example: jmax -myProperty "myValue"

if {[systemProperty "myProperty"] == "myValue"} { 
  # property dependent code here
}

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

#    puts "Irix 6.2 default audio configuration: Analog 2/2 @ [jmaxGetSampleRate] ([jmaxGetAudioBuffer])"
#    ucs open device out~ default as SgiALOut channels 2
#    ucs open device in~ default as SgiALIn channels 2 
#    
#    ucs default out~ default 
#    ucs default in~ default

} elseif {$jmaxArch == "o2r5k" || $jmaxArch == "o2r10k"} {

#    puts "O2 default audio configuration: Analog 2/2 @ [jmaxGetSampleRate] ([jmaxGetAudioBuffer])"
#    ucs open device in~ default as SgiALIn ALdevice AnalogIn channels 2 
#    ucs open device out~ default as SgiALOut ALdevice AnalogOut channels 2 

#    ucs default in~ default
#    ucs default out~ default

} elseif {$jmaxArch == "origin"} {

# analog
#    puts "origin default audio configuration: Analog 2/2 @ [jmaxGetSampleRate] ([jmaxGetAudioBuffer])"
#    ucs open device in~ default as SgiALIn ALdevice AnalogIn channels 2 
#    ucs open device out~ default as SgiALOut ALdevice AnalogOut channels 2 

# ADAT
#    puts "origin default audio configuration: ADAT 8/8 @ [jmaxGetSampleRate] ([jmaxGetAudioBuffer])"
#    ucs open device in~ default as SgiALIn ALdevice ADATIn channels 8 
#    ucs open device out~ default as SgiALOut ALdevice ADATOut channels 8

# AES
#    puts "origin default audio configuration: AES 2/2 @ [jmaxGetSampleRate] ([jmaxGetAudioBuffer])"
#    ucs open device in~ default as SgiALIn ALdevice AESIn channels 2 
#    ucs open device out~ default as SgiALOut ALdevice AESOut channels 2

#    ucs default in~ default
#    ucs default out~ default

} elseif {$jmaxArch == "linuxpc"} {

#    puts "OSS audio configuration: 2/2
#    ucs open device out~ default as oss_dac
#    ucs open device in~ default as oss_adc
   
#    ucs default out~ default 
#    ucs default in~ default

} else {

    puts "unknow architecture (profile DACs installed)"

    ucs open device out~ prof_out as prof_dac channels 2

    ucs default out~ prof_out
}








