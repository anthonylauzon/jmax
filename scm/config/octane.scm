;;
;; jMax
;; Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
;; 
;; This program is free software; you can redistribute it and;or
;; modify it under the terms of the GNU General Public License
;; as published by the Free Software Foundation; either version 2
;; of the License, or (at your option) any later version.
;; 
;; See file LICENSE for further informations on licensing terms.
;; 
;; This program is distributed in the hope that it will be useful,
;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;; GNU General Public License for more details.
;; 
;; You should have received a copy of the GNU General Public License
;; along with this program; if not, write to the Free Software
;; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
;; 
;; Based on Max;ISPW by Miller Puckette.
;; 
;; Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;;
(set! jmax-arch "irix65r10k")

(define-device "analogIn"  (list "SgiALIn" "ALdevice" "AnalogIn" "channels" "2")   "Analog In")
(define-device "analogOut" (list "SgiALOut" "ALdevice" "AnalogOut" "channels" "2")    "Analog Out")

(define-device "stereoIn"  (list "SgiALIn" "ALdevice" "RAD1.AESIn" "channels" "2") "RAD 1.AES In")
(define-device "stereoIn1" (list "SgiALIn" "ALdevice" "RAD1.AESIn" "channels" "2") "RAD 1.AES In")
(define-device "stereoIn2" (list "SgiALIn" "ALdevice" "RAD2.AESIn" "channels" "2") "RAD 2.AES In")
(define-device "stereoIn3" (list "SgiALIn" "ALdevice" "RAD3.AESIn" "channels" "2") "RAD 3.AES In")

(define-device "stereoOut"  (list "SgiALOut" "ALdevice" "RAD1.AESOut" "channels" "2") "RAD 1.AES Out")
(define-device "stereoOut1" (list "SgiALOut" "ALdevice" "RAD1.AESOut" "channels" "2") "RAD 1.AES Out")
(define-device "stereoOut2" (list "SgiALOut" "ALdevice" "RAD2.AESOut" "channels" "2") "RAD 2.AES Out")
(define-device "stereoOut3" (list "SgiALOut" "ALdevice" "RAD3.AESOut" "channels" "2") "RAD 3.AES Out")

(define-device "adatIn"  (list "SgiALIn" "ALdevice" "ADATIn" "channels" "8")      "ADAT In")
(define-device "adatIn1" (list "SgiALIn" "ALdevice" "RAD1.ADATIn" "channels" "8") "RAD 1.ADAT In")
(define-device "adatIn2" (list "SgiALIn" "ALdevice" "RAD2.ADATIn" "channels" "8") "RAD 2.ADAT In")
(define-device "adatIn3" (list "SgiALIn" "ALdevice" "RAD3.ADATIn" "channels" "8") "RAD 3.ADAT In")

(define-device "adatOut"  (list "SgiALOut" "ALdevice" "ADATOut" "channels" "8")      "ADAT Out")
(define-device "adatOut1" (list "SgiALOut" "ALdevice" "RAD1.ADATOut" "channels" "8") "RAD 1.ADAT Out")
(define-device "adatOut2" (list "SgiALOut" "ALdevice" "RAD2.ADATOut" "channels" "8") "RAD 2.ADAT Out")
(define-device "adatOut3" (list "SgiALOut" "ALdevice" "RAD3.ADATOut" "channels" "8") "RAD 3.ADAT Out")

(define-device "midi"  (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi1" (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi2" (list "sgi_midi" "port" "SerialPort2") "MIDI on startmidi port SerialPort2")

(define (platform-start-hook)
  (require-package "sgidev" "0.0.0")
  (require-package "pdtddev" "0.0.0")
)
(define-hook "platform-start" platform-start-hook)

(define (default-start-kook)
  (open-default-audio-in "analogIn")
  (open-default-audio-out "analogOut")
)
(define-hook "default-start" default-start-hook)