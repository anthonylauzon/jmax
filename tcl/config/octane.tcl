#
#  jMax
#  
#  Copyright (C) 1999 by IRCAM
#  All rights reserved.
#  
#  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#
#  This program may be used and distributed under the terms of the 
#  accompanying LICENSE.
# 
#  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
#  for DISCLAIMER OF WARRANTY.
#  
set jmaxArch irix65r10k

defineDevice analogIn  { SgiALIn ALdevice AnalogIn channels 2 }   "Analog In"
defineDevice analogOut  { SgiALOut ALdevice AnalogOut channels 2 }    "Analog Out"

defineDevice stereoIn  { SgiALIn ALdevice RAD1.AESIn channels 2 } "RAD 1.AES In"
defineDevice stereoIn1 { SgiALIn ALdevice RAD1.AESIn channels 2 } "RAD 1.AES In"
defineDevice stereoIn2 { SgiALIn ALdevice RAD2.AESIn channels 2 } "RAD 2.AES In"
defineDevice stereoIn3 { SgiALIn ALdevice RAD3.AESIn channels 2 } "RAD 3.AES In"

defineDevice stereoOut  { SgiALOut ALdevice RAD1.AESOut channels 2 } "RAD 1.AES Out"
defineDevice stereoOut1 { SgiALOut ALdevice RAD1.AESOut channels 2 } "RAD 1.AES Out"
defineDevice stereoOut2 { SgiALOut ALdevice RAD2.AESOut channels 2 } "RAD 2.AES Out"
defineDevice stereoOut3 { SgiALOut ALdevice RAD3.AESOut channels 2 } "RAD 3.AES Out"

defineDevice adatIn  { SgiALIn ALdevice ADATIn channels 8 }      "ADAT In"
defineDevice adatIn1 { SgiALIn ALdevice RAD1.ADATIn channels 8 } "RAD 1.ADAT In"
defineDevice adatIn2 { SgiALIn ALdevice RAD2.ADATIn channels 8 } "RAD 2.ADAT In"
defineDevice adatIn3 { SgiALIn ALdevice RAD3.ADATIn channels 8 } "RAD 3.ADAT In"

defineDevice adatOut  { SgiALOut ALdevice ADATOut channels 8 }      "ADAT Out"
defineDevice adatOut1 { SgiALOut ALdevice RAD1.ADATOut channels 8 } "RAD 1.ADAT Out"
defineDevice adatOut2 { SgiALOut ALdevice RAD2.ADATOut channels 8 } "RAD 2.ADAT Out"
defineDevice adatOut3 { SgiALOut ALdevice RAD3.ADATOut channels 8 } "RAD 3.ADAT Out"

defineDevice midi  { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi1 { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi2 { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"

