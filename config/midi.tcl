##################################################################
#
#  FTS default MIDI device configuration
#
#    mips1
#    irix5.3
#    irix6.2
#    o2r5k and o2r10k
#    origin
#

###### mips1 irix5.3 irix6.2 origin ######

if {$ftsArch == mips1 || $ftsArch == irix5.3 || origin} {
    post "FTS default MIDI configuration"
    post "(make sure that you lauched the MIDI deamon)"
    post "  port: FTSmidi"
    ucs open device midi 0 as sgi_midi port FTSmidi
} 

###### o2r5k and o2r10k ######

elseif {$ftsArch == o2r5k || $ftsArch == o2r10k} {
    post (MIDI not configured)
}

# shell commands for startmidi

# origin
#   serial port 1: startmidi -d /dev/tty4d1 -s 31250 -n FTSmidi
#   serial port 2: startmidi -d /dev/tty4d2 -s 31250 -n FTSmidi

# mips1, irix5.3 and irix 6.2
#   serial port 1: startmidi -d /dev/ttyd1 -n FTSmidi
#   serial port 2: startmidi -d /dev/ttyd2 -n FTSmidi

# MIDI is not yet supported on O2 (o2r5k and o2r10k)
