######################################################################################
#
#  jMax default startup MIDI configuration
#
#  ... everything here can depend on the following system properties:

global jmaxArch 
global jmaxHost
global jmaxMidiPort

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
#   shell commands for startmidi using the default MIDI Port (SerialPort2)
#
#     origin
#       serial port 1: startmidi -d /dev/ttyd1 -s 31250 -n SerialPort2
#       serial port 2: startmidi -d /dev/ttyd2 -s 31250 -n SerialPort2
#
#     irix 6.2
#       serial port 1: startmidi -d /dev/ttyd1 -n SerialPort2
#       serial port 2: startmidi -d /dev/ttyd2 -n SerialPort2
#
#   MIDI is not yet supported on O2 (o2r5k and o2r10k)
#

if {$jmaxArch == "origin" || $jmaxArch == "irix6.2"} {

    puts "MIDI configuration: $jmaxMidiPort"
    ucs open device midi 0 as sgi_midi port $jmaxMidiPort

}


