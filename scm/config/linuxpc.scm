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

(set! jmax-arch "linuxpc")

(define-device "analogIn"  "oss_adc" "OSS audio in")
(define-device "analogOut" "oss_dac" "OSS audio out")

(define-device "stereoIn"  "oss_adc" "OSS audio in")
(define-device "stereoIn1" "oss_adc" "OSS audio in")

(define-device "studioOutA" (list "oss_dac" "device" "/dev/dsp1" "channels" "8") "StudI/O output ADAT port A")
(define-device "studioInA" (list "oss_adc" "device" "/dev/dsp11" "channels" "2") "StudI/O input ADAT port A")

(define-device "stereoOut"  "oss_dac" "OSS audio out")
(define-device "stereoOut1" "oss_dac" "OSS audio out")

(define-device "midi" "oss_midi" "OSS MIDI")

