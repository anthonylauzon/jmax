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
set jmaxHostArch(myHost) "hisArch"
set jmaxHostArch(anotherHost) "anotherArch"


####################################
## default audio/MIDI parameters

set jmaxDefaultSampleRate 44100
set jmaxDefaultAudioBuffer 2048
set jmaxDefaultMidiPort "jmaxMidi"


####################################
## very expert settings

## set jMax client/server connection mode
set jmaxDefaultConnection "client"
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
set Arch [exec arch]
if {$Arch == "Linux"} {
  set jmaxDefaultArch "pclinux"
} elseif {$Arch == "sgi"} {
  set jmaxDefaultArch "irix6.2"
} else {
  set jmaxDefaultArch ""
}
  

