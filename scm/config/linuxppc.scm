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

(set! jmax-arch "linuxpc")

(define-device "analogIn"  "oss_adc" "OSS audio in")
(define-device "analogOut" "oss_dac" "OSS audio out")

(define-device "stereoIn"  "oss_adc" "OSS audio in")
(define-device "stereoIn1" "oss_adc" "OSS audio in")

(define-device "stereoOut"  "oss_dac" "OSS audio out")
(define-device "stereoOut1" "oss_dac" "OSS audio out")

(define-device "midi" "oss_midi" "OSS MIDI")

(when "platform-start" "(platform-start-hook)")

(define (platform-start-hook)
  (require-package "ossdev" "0.0.0")
  (require-package "pdtddev" "0.0.0")
)

(when "default-start" "(default-start-kook)")

(define (default-start-kook)
;;  (open-default-audio-in "stereoIn")
;;  (open-default-audio-out "stereoOut")
;;  (open-default-midi "midi")
)
