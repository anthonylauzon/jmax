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

defineDevice sonorusIn12  { oss_adc device /dev/dsp1} "Sonorus in 1 & 2"
defineDevice sonorusOut12  { oss_dac device /dev/dsp1} "Sonorus out 1 & 2"

defineDevice stereoOut  { oss_dac } "OSS audio out"
defineDevice stereoOut1 { oss_dac } "OSS audio out"

defineDevice midi { oss_midi } "OSS MIDI"

