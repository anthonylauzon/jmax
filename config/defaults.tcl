#################################################################
#
#  jMax installation defaults of the system properties
#  
#  please modifiy these for your local installation
#

####################################
## default host settings

## jMax servers default host name
set jmaxDefaultHost [exec hostname]

## set jMax servers host architectures (array of host names)
# (IRCAM's SGI machines)
set jmaxHostArch(astor) "origin"
set jmaxHostArch(mimi) "origin"
set jmaxHostArch(octane) "origin"
set jmaxHostArch(bonang) "o2r10k"
set jmaxHostArch(marimba) "o2r5k"
set jmaxHostArch(tabla) "o2r5k"
set jmaxHostArch(banya) "irix6.2"
set jmaxHostArch(brian) "irix6.2"


####################################
## default audio/MIDI parameters

set jmaxDefaultSampleRate 44100
set jmaxDefaultAudioBuffer 2048
set jmaxDefaultMidiPort "jmaxMidi"


####################################
## very expert settings

## set jMax client/server connection mode
set jmaxDefaultConnection "udp"
#set jmaxDefaultConnection "client"
#set jmaxDefaultConnection "socket"
#set jmaxDefaultConnection "local"

## set binary execution mode for jMax server executable
set jmaxDefaultMode "opt"
#set jmaxDefaultMode "debug"

## set name for jMax server executable
set jmaxDefaultServerName "fts"

## set port number needed for socket connection
set jmaxDefaultPort 2000

## default user package path
set jmaxDefaultPkgPath "[systemProperty user.home]/jmax"

## default architecture if not specified for host name
## just works if server runns on same machine as client

set jmaxDefaultArch "irix6.2"

#set Arch [exec arch]
#if {$Arch == "Linux"} {
#  set jmaxDefaultArch "pclinux"
#} elseif {$Arch == "sgi"} {
#  set jmaxDefaultArch "irix6.2"
#} else {
#  set jmaxDefaultArch ""
#}

  

