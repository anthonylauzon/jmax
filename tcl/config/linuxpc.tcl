set jmaxArch linuxpc

defineDevice analogIn  { oss_adc device /dev/audio } "OSS audio in"
defineDevice analogOut { oss_dac } "OSS audio out"

defineDevice stereoIn  { oss_adc device /dev/audio } "OSS audio in"
defineDevice stereoIn1 { oss_adc device /dev/audio } "OSS audio in"

defineDevice stereoOut  { oss_dac device /dev/audio } "OSS audio out"
defineDevice stereoOut1 { oss_dac device /dev/audio } "OSS audio out"

