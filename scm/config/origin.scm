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

(define-device "stereoIn"  (list "SgiALIn" "ALdevice" "RAD1.AESIn" "channels" "2") "RAD 1.AES In")
(define-device "stereoIn1" (list "SgiALIn" "ALdevice" "RAD1.AESIn" "channels" "2") "RAD 1.AES In")
(define-device "stereoIn2" (list "SgiALIn" "ALdevice" "RAD2.AESIn" "channels" "2") "RAD 2.AES In")
(define-device "stereoIn3" (list "SgiALIn" "ALdevice" "RAD3.AESIn" "channels" "2") "RAD 3.AES In")
(define-device "stereoIn4" (list "SgiALIn" "ALdevice" "RAD4.AESIn" "channels" "2") "RAD 4.AES In")
(define-device "stereoIn5" (list "SgiALIn" "ALdevice" "RAD5.AESIn" "channels" "2") "RAD 5.AES In")
(define-device "stereoIn6" (list "SgiALIn" "ALdevice" "RAD6.AESIn" "channels" "2") "RAD 6.AES In")

(define-device "stereoOut"  (list "SgiALOut" "ALdevice" "RAD1.AESOut" "channels" "2") "RAD 1.AES Out")
(define-device "stereoOut1" (list "SgiALOut" "ALdevice" "RAD1.AESOut" "channels" "2") "RAD 1.AES Out")
(define-device "stereoOut2" (list "SgiALOut" "ALdevice" "RAD2.AESOut" "channels" "2") "RAD 2.AES Out")
(define-device "stereoOut3" (list "SgiALOut" "ALdevice" "RAD3.AESOut" "channels" "2") "RAD 3.AES Out")
(define-device "stereoOut4" (list "SgiALOut" "ALdevice" "RAD4.AESOut" "channels" "2") "RAD 4.AES Out")
(define-device "stereoOut5" (list "SgiALOut" "ALdevice" "RAD5.AESOut" "channels" "2") "RAD 5.AES Out")
(define-device "stereoOut6" (list "SgiALOut" "ALdevice" "RAD6.AESOut" "channels" "2") "RAD 6.AES Out")

(define-device "adatIn"  (list "SgiALIn" "ALdevice" "ADATIn" "channels" "8")      "RAD 1.ADAT In")
(define-device "adatIn1" (list "SgiALIn" "ALdevice" "RAD1.ADATIn" "channels" "8") "RAD 1.ADAT In")
(define-device "adatIn2" (list "SgiALIn" "ALdevice" "RAD2.ADATIn" "channels" "8") "RAD 2.ADAT In")
(define-device "adatIn3" (list "SgiALIn" "ALdevice" "RAD3.ADATIn" "channels" "8") "RAD 3.ADAT In")
(define-device "adatIn4" (list "SgiALIn" "ALdevice" "RAD4.ADATIn" "channels" "8") "RAD 4.ADAT In")
(define-device "adatIn5" (list "SgiALIn" "ALdevice" "RAD5.ADATIn" "channels" "8") "RAD 5.ADAT In")
(define-device "adatIn6" (list "SgiALIn" "ALdevice" "RAD6.ADATIn" "channels" "8") "RAD 6.ADAT In")

(define-device "adatOut"  (list "SgiALOut" "ALdevice" "ADATOut" "channels" "8")      "RAD 1.ADAT Out")
(define-device "adatOut1" (list "SgiALOut" "ALdevice" "RAD1.ADATOut" "channels" "8") "RAD 1.ADAT Out")
(define-device "adatOut2" (list "SgiALOut" "ALdevice" "RAD2.ADATOut" "channels" "8") "RAD 2.ADAT Out")
(define-device "adatOut3" (list "SgiALOut" "ALdevice" "RAD3.ADATOut" "channels" "8") "RAD 3.ADAT Out")
(define-device "adatOut4" (list "SgiALOut" "ALdevice" "RAD4.ADATOut" "channels" "8") "RAD 4.ADAT Out")
(define-device "adatOut5" (list "SgiALOut" "ALdevice" "RAD5.ADATOut" "channels" "8") "RAD 5.ADAT Out")
(define-device "adatOut6" (list "SgiALOut" "ALdevice" "RAD6.ADATOut" "channels" "8") "RAD 6.ADAT Out")

(define-device "midi"  (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi1" (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi2" (list "sgi_midi" "port" "SerialPort2") "MIDI on startmidi port SerialPort2")
(define-device "midi3" (list "sgi_midi" "port" "SerialPort3") "MIDI on startmidi port SerialPort3")
(define-device "midi4" (list "sgi_midi" "port" "SerialPort4") "MIDI on startmidi port SerialPort4")

(define (platform-start-hook)
  (require-package "sgidev" "0.0.0")
  (require-package "pdtddev" "0.0.0")
)
(define-hook "platform-start" platform-start-hook)


(define (default-start-hook)
  (open-default-audio-in "adatIn")
  (open-default-audio-out "adatOut")
)
(define-hook "default-start" default-start-hook)

