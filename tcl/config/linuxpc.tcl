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
set jmaxArch linuxpc

defineDevice analogIn  { oss_adc } "OSS audio in"
defineDevice analogOut { oss_dac } "OSS audio out"

defineDevice stereoIn  { oss_adc } "OSS audio in"
defineDevice stereoIn1 { oss_adc } "OSS audio in"

defineDevice studioOutA { oss_dac device /dev/dsp1 channels 8 } "StudI/O output ADAT port A"
defineDevice studioInA { oss_adc device /dev/dsp11 channels 2 } "StudI/O input ADAT port A"

defineDevice stereoOut  { oss_dac } "OSS audio out"
defineDevice stereoOut1 { oss_dac } "OSS audio out"

defineDevice midi { oss_midi } "OSS MIDI"

