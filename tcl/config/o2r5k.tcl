set jmaxArch irix65r5k

defineDevice analogIn   { SgiALIn ALdevice AnalogIn channels 2 }  "Analog In"
defineDevice analogOut  { SgiALOut ALdevice AnalogOut channels 2 } "Analog Out"
defineDevice analogOut1 { SgiALOut ALdevice AnalogOut channels 2 } "Analog Out"
defineDevice analogOut2  { SgiALOut ALdevice AnalogOut2 channels 2 } "Analog Out 2"

defineDevice stereoIn  { SgiALIn ALdevice RAD1.AESIn channels 2 } "AES In"
defineDevice stereoIn1 { SgiALIn ALdevice RAD1.AESIn channels 2 } "AES In"

defineDevice stereoOut  { SgiALOut ALdevice RAD1.AESOut channels 2 } "AES Out"
defineDevice stereoOut1 { SgiALOut ALdevice RAD1.AESOut channels 2 } "AES Out"

defineDevice adatIn  { SgiALIn ALdevice ADATIn channels 8 }      "ADAT In"
defineDevice adatIn1 { SgiALIn ALdevice RAD1.ADATIn channels 8 } "ADAT In"

defineDevice adatOut  { SgiALOut ALdevice ADATOut channels 8 }      "ADAT Out"
defineDevice adatOut1 { SgiALOut ALdevice RAD1.ADATOut channels 8 } "ADAT Out"

defineDevice midi  { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi1 { sgi_midi port SerialPort1 } "MIDI on startmidi port SerialPort1"
defineDevice midi2 { sgi_midi port SerialPort2 } "MIDI on startmidi port SerialPort2"

