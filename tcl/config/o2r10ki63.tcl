set jmaxArch irix63r10k

defineDevice analogIn   { SgiALIn ALdevice AnalogIn channels 2 }  "Builtin Analog In"
defineDevice analogOut  { SgiALIn ALdevice AnalogOut channels 2 } "Builtin Analog Out"

defineDevice stereoIn  { SgiALIn ALdevice RAD1.AESIn channels 2 } "Stereo AES card 1"
defineDevice stereoIn0 { SgiALIn ALdevice RAD1.AESIn channels 2 } "Stereo AES card 1"

defineDevice stereoOut  { SgiALOut ALdevice RAD1.AESOut channels 2 } "Stereo AES card 1"
defineDevice stereoOut0 { SgiALOut ALdevice RAD1.AESOut channels 2 } "Stereo AES card 1"

defineDevice adatIn   { SgiALIn ALdevice RAD1.ADATIn channels 8 } "8 channel ADAT card 1"
defineDevice adatIn1  { SgiALIn ALdevice RAD1.ADATIn channels 8 } "8 channel ADAT card 1"

defineDevice adatOut   { SgiALOut ALdevice RAD1.ADATOut channels 8 } "8 channel ADAT card 1"
defineDevice adatOut1  { SgiALOut ALdevice RAD1.ADATOut channels 8 } "8 channel ADAT card 1"

defineDevice midi      { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"

