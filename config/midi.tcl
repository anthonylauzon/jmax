##################################################################
#
#  jMax default MIDI device configuration
#
#    mips1
#    irix5.3
#    irix6.2
#    o2r5k and o2r10k
#    origin
#


global jmaxArch jmaxMidiPort

if {$jmaxArch == "mips1" || $jmaxArch == "irix5.3" || $jmaxArch == "origin"} {
    puts "jMax default MIDI configuration"
    puts "(make sure that you lauched the MIDI deamon using the startmidi command)"
    puts "  port: $jmaxMidiPort"
    ucs open device midi 0 as sgi_midi port $jmaxMidiPort
} elseif {$jmaxArch == "o2r5k" || $jmaxArch == "o2r10k"} {
    puts "(MIDI not configured)"
}

# shell commands for startmidi

# origin
#   serial port 1: startmidi -d /dev/tty4d1 -s 31250 -n FTSmidi
#   serial port 2: startmidi -d /dev/tty4d2 -s 31250 -n FTSmidi

# mips1, irix5.3 and irix 6.2
#   serial port 1: startmidi -d /dev/ttyd1 -n FTSmidi
#   serial port 2: startmidi -d /dev/ttyd2 -n FTSmidi

# MIDI is not yet supported on O2 (o2r5k and o2r10k)
