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

set jmaxArch irix65r4k

defineDevice analogIn   { SgiALIn ALdevice AnalogIn channels 2 }  "Analog In"
defineDevice analogOut  { SgiALOut ALdevice AnalogOut channels 2 } "Analog Out"

defineDevice digitalIn  { SgiALIn ALdevice DigitalIn channels 2 } "Digital In"
defineDevice digitalOut  { SgiALOut ALdevice DigitalOut channels 2 } "Digital Out"

defineDevice midi  { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi1 { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi2 { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"

