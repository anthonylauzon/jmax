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

defineDevice stereoIn  { SgiALIn ALdevice RAD1.AESIn channels 2 } "RAD 1.AES In"
defineDevice stereoIn1 { SgiALIn ALdevice RAD1.AESIn channels 2 } "RAD 1.AES In"
defineDevice stereoIn2 { SgiALIn ALdevice RAD2.AESIn channels 2 } "RAD 2.AES In"
defineDevice stereoIn3 { SgiALIn ALdevice RAD3.AESIn channels 2 } "RAD 3.AES In"
defineDevice stereoIn4 { SgiALIn ALdevice RAD4.AESIn channels 2 } "RAD 4.AES In"
defineDevice stereoIn5 { SgiALIn ALdevice RAD5.AESIn channels 2 } "RAD 5.AES In"
defineDevice stereoIn6 { SgiALIn ALdevice RAD6.AESIn channels 2 } "RAD 6.AES In"

defineDevice stereoOut  { SgiALOut ALdevice RAD1.AESOut channels 2 } "RAD 1.AES Out"
defineDevice stereoOut1 { SgiALOut ALdevice RAD1.AESOut channels 2 } "RAD 1.AES Out"
defineDevice stereoOut2 { SgiALOut ALdevice RAD2.AESOut channels 2 } "RAD 2.AES Out"
defineDevice stereoOut3 { SgiALOut ALdevice RAD3.AESOut channels 2 } "RAD 3.AES Out"
defineDevice stereoOut4 { SgiALOut ALdevice RAD4.AESOut channels 2 } "RAD 4.AES Out"
defineDevice stereoOut5 { SgiALOut ALdevice RAD5.AESOut channels 2 } "RAD 5.AES Out"
defineDevice stereoOut6 { SgiALOut ALdevice RAD6.AESOut channels 2 } "RAD 6.AES Out"

defineDevice adatIn  { SgiALIn ALdevice ADATIn channels 8 }      "RAD 1.ADAT In"
defineDevice adatIn1 { SgiALIn ALdevice RAD1.ADATIn channels 8 } "RAD 1.ADAT In"
defineDevice adatIn2 { SgiALIn ALdevice RAD2.ADATIn channels 8 } "RAD 2.ADAT In"
defineDevice adatIn3 { SgiALIn ALdevice RAD3.ADATIn channels 8 } "RAD 3.ADAT In"
defineDevice adatIn4 { SgiALIn ALdevice RAD4.ADATIn channels 8 } "RAD 4.ADAT In"
defineDevice adatIn5 { SgiALIn ALdevice RAD5.ADATIn channels 8 } "RAD 5.ADAT In"
defineDevice adatIn6 { SgiALIn ALdevice RAD6.ADATIn channels 8 } "RAD 6.ADAT In"

defineDevice adatOut  { SgiALOut ALdevice ADATOut channels 8 }      "RAD 1.ADAT Out"
defineDevice adatOut1 { SgiALOut ALdevice RAD1.ADATOut channels 8 } "RAD 1.ADAT Out"
defineDevice adatOut2 { SgiALOut ALdevice RAD2.ADATOut channels 8 } "RAD 2.ADAT Out"
defineDevice adatOut3 { SgiALOut ALdevice RAD3.ADATOut channels 8 } "RAD 3.ADAT Out"
defineDevice adatOut4 { SgiALOut ALdevice RAD4.ADATOut channels 8 } "RAD 4.ADAT Out"
defineDevice adatOut5 { SgiALOut ALdevice RAD5.ADATOut channels 8 } "RAD 5.ADAT Out"
defineDevice adatOut6 { SgiALOut ALdevice RAD6.ADATOut channels 8 } "RAD 6.ADAT Out"

defineDevice midi  { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi1 { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi2 { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"
defineDevice midi3 { sgi_midi port SerialPort3 } "MIDI on startmidi port SerialPort3"
defineDevice midi4 { sgi_midi port SerialPort4 } "MIDI on startmidi port SerialPort4"

