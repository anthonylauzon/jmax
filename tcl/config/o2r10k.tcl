# 
# jMax
# Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
# 
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
# 
# See file LICENSE for further informations on licensing terms.
# 
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
# 
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
# 
# Based on Max/ISPW by Miller Puckette.
#
# Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
#

set jmaxArch irix65r10k

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

when platformStart {
    package require sgidev
    package require pdtddev
}

when defaultStart {
    openDefaultAudioIn analogIn
    openDefaultAudioOut analogOut
}

