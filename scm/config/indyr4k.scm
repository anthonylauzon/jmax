;;
;;  jMax
;;  
;;  Copyright (C) 1999 by IRCAM
;;  All rights reserved.
;;  
;;  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;;
;;  This program may be used and distributed under the terms of the 
;;  accompanying LICENSE.
;; 
;;  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
;;  for DISCLAIMER OF WARRANTY.
;;  

(set! jmax-arch "irix65r4k")

(define-device "analogIn"    (list "SgiALIn" "ALdevice" "AnalogIn" "channels" "2")  "Analog In")
(define-device "analogOut"   (list "SgiALOut" "ALdevice" "AnalogOut" "channels" "2") "Analog Out")

(define-device "digitalIn"   (list "SgiALIn" "ALdevice" "DigitalIn" "channels" "2") "Digital In")
(define-device "digitalOut"  (list "SgiALOut" "ALdevice" "DigitalOut" "channels" "2") "Digital Out")

(define-device "midi"  (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi1" (list "sgi_midi" "port" "SerialPort1") "MIDI on startmidi port SerialPort1")
(define-device "midi2" (list "sgi_midi" "port" "SerialPort2") "MIDI on startmidi port SerialPort2")

