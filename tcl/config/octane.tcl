set jmaxArch irix65r10k

defineDevice analogIn   { SgiALIn ALdevice AnalogIn channels 2 }  "Builtin Analog In"
defineDevice analogOut  { SgiALIn ALdevice AnalogOut channels 2 } "Builtin Analog Out"

defineDevice stereoIn  { SgiALIn ALdevice AESIn channels 2 }      "Builtin AES In"
defineDevice stereoIn0 { SgiALIn ALdevice AESIn channels 2 }      "Builtin AES In"
defineDevice stereoIn1 { SgiALIn ALdevice RAD1.AESIn channels 2 } "Stereo AES card 1"
defineDevice stereoIn2 { SgiALIn ALdevice RAD2.AESIn channels 2 } "Stereo AES card 2"
defineDevice stereoIn3 { SgiALIn ALdevice RAD3.AESIn channels 2 } "Stereo AES card 3"


defineDevice stereoOut  { SgiALOut ALdevice AESOut channels 2 }      "Builtin AES Out"
defineDevice stereoOut0 { SgiALOut ALdevice AESOut channels 2 }      "Builtin AES Out"
defineDevice stereoOut1 { SgiALOut ALdevice RAD1.AESOut channels 2 } "Stereo AES card 1"
defineDevice stereoOut2 { SgiALOut ALdevice RAD2.AESOut channels 2 } "Stereo AES card 2"
defineDevice stereoOut3 { SgiALOut ALdevice RAD3.AESOut channels 2 } "Stereo AES card 3"

defineDevice adatIn   { SgiALIn ALdevice ADATIn channels 8 }      "Built 8 channel ADAT"
defineDevice adatIn0  { SgiALIn ALdevice ADATIn channels 8 }      "Built 8 channel ADAT"
defineDevice adatIn1  { SgiALIn ALdevice RAD1.ADATIn channels 8 } "8 channel ADAT card 1"
defineDevice adatIn2  { SgiALIn ALdevice RAD2.ADATIn channels 8 } "8 channel ADAT card 2"
defineDevice adatIn3  { SgiALIn ALdevice RAD3.ADATIn channels 8 } "8 channel ADAT card 3"


defineDevice adatOut   { SgiALOut ALdevice ADATOut channels 8 }      "Builtin 8 channel ADAT"
defineDevice adatOut0  { SgiALOut ALdevice ADATOut channels 8 }      "Builtin 8 channel ADAT"
defineDevice adatOut1  { SgiALOut ALdevice RAD1.ADATOut channels 8 } "8 channel ADAT card 1"
defineDevice adatOut2  { SgiALOut ALdevice RAD2.ADATOut channels 8 } "8 channel ADAT card 2"
defineDevice adatOut3  { SgiALOut ALdevice RAD3.ADATOut channels 8 } "8 channel ADAT card 3"


defineDevice midi      { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"

