#
#  jMax site configuration
#  
#  This file is executed at start up; you can add tcl code
#  here to simplify the user configuration in your site
#
#  For example, for each type of machines you can write a tcl
#  function that set the standard configuration for your site,
#  and your users can just execute this function in their .jmaxrc
#  
#  Like:
#
#  proc octaneStandardConfig {} {
# 	openDefaultMidi     SerialPort2
#	openDefaultAudioIn  ADATIn   8 
#	openDefaultAudioOut ADATOut  8     
#  }
#
#
# Also, in order to simplify your users .jmaxrc file, you can fill up
# a tcl array called jmaxHostTypeTable, specifing the host type of all your
# machines; what follow, as an example, is the current host type configuration
# in Ircam

set jmaxHostTypeTable("astor.ircam.fr")      origin
set jmaxHostTypeTable("stradivari.ircam.fr") octane
set jmaxHostTypeTable("maggini.ircam.fr")    octane
set jmaxHostTypeTable("amati.ircam.fr")      octane
set jmaxHostTypeTable("marimba.ircam.fr")    o2r5ki63
set jmaxHostTypeTable("libido.ircam.fr")     linuxpc
set jmaxHostTypeTable("linotte.ircam.fr")    linuxpc
