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
set jmaxArch linuxpc

defineDevice analogIn  { oss_adc } "OSS audio in"
defineDevice analogOut { oss_dac } "OSS audio out"

defineDevice stereoIn  { oss_adc } "OSS audio in"
defineDevice stereoIn1 { oss_adc } "OSS audio in"

defineDevice sonorusIn12  { oss_adc device /dev/dsp1} "sonorus in 1 & 2"
defineDevice stereoOut1 { SgiALOut ALdevice RAD1.AESOut channels 2 } "RAD 1.AES Out"
defineDevice stereoOut2 { SgiALOut ALdevice RAD2.AESOut channels 2 } "RAD 2.AES Out"
defineDevice stereoOut3 { SgiALOut ALdevice RAD3.AESOut channels 2 } "RAD 3.AES Out"
defineDevice stereoOut4 { SgiALOut ALdevice RAD4.AESOut channels 2 } "RAD 4.AES Out"
defineDevice stereoOut5 { SgiALOut ALdevice RAD5.AESOut channels 2 } "RAD 5.AES Out"
defineDevice stereoOut6 { SgiALOut ALdevice RAD6.AESOut channels 2 } "RAD 6.AES Out"

defineDevice stereoOut  { oss_dac } "OSS audio out"
defineDevice stereoOut1 { oss_dac } "OSS audio out"

defineDevice midi { oss_midi } "OSS MIDI"

