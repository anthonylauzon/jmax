#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  

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
# 	openDefaultMidi     "SerialPort2"
#	openDefaultAudioIn  ADATIn   8 
#	openDefaultAudioOut ADATOut  8     
#  }
#
#
# Also, in order to simplify your users .jmaxrc file, you can declare the host type
# of your hosts by using the function defineHostType <host> <type>

defineHost "astor.ircam.fr"      origin
defineHost "stradivari.ircam.fr" octane
defineHost "maggini.ircam.fr"    octane
defineHost "amati.ircam.fr"      octane
defineHost "marimba.ircam.fr"    o2r5ki63
defineHost "libido.ircam.fr"     linuxpc
defineHost "linotte.ircam.fr"    linuxpc

