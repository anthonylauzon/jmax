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
;; Author: Peter Hanappe
;;

;;
;; Simple functions to simplify Sgi configuration
;; 

;;
;; The device list
;;
(define jmax-device-list ())

;;
;; define-device
;;
(define (define-device name ucscode doc)
  (set! jmax-device-list (cons (list name ucscode doc) jmax-device-list)))

;;
;; get-device
;;
(define (get-device name)
  (get-device-recur name jmax-device-list)) 

;;
;; get-device-recur
;;
(define (get-device-recur name list)
  (if (null? list)
      ()
      (if (string=? name (caar list))
	  (cadar list)
	  (get-device-recur name (cdr list)))))

;;
;; get-device-doc
;;
(define (get-device-doc name)
  (get-device-doc-recur name jmax-device-list)) 

;;
;; get-device-doc-recur
;;
(define (get-device-doc-recur name list)
  (if (null? list)
      ()
      (if (string=? name (caar list))
	  (caddar list)
	  (get-device-doc-recur name (cdr list)))))

;;
;; reset-audio-in
;;
(define (reset-audio-in)
  (ucs "reset" "device" "in~"))

;;
;; open-audio-in
;;
(define (open-audio-in name device)
  (println "Opening audio in " name ": " (get-device-doc device))
  (apply ucs (append (list "open" "device" "in~" name "as") (get-device device))))

;;
;; open-default-audio-in
;;
(define (open-default-audio-in device)
  (println "Opening default audio in: " (get-device-doc device))
  (apply ucs (append (list "open" "device" "in~" "__defaultAudioIn" "as") (get-device device)))
  (ucs "default" "in~" "__defaultAudioIn"))

;;
;; open-named-default-audio-in
;;
(define (open-named-default-audio-in name device)
  (println "Opening audio in " name "(also default): " (get-device-doc device))
  (apply ucs (append (list "open" "device" "in~" name "as") (get-device device)))
  (ucs "default" "in~" name))

;;
;; reset-audio-out
;;
(define (reset-audio-out)
  (ucs "reset" "device" "out~"))

;;
;; open-audio-out
;;
(define (open-audio-out name device)
  (println "Opening audio out " name ": " (get-device-doc device))
  (apply ucs (append (list "open" "device" "out~" name "as") (get-device device))))

;;
;; open-default-audio-out
;;
(define (open-default-audio-out device)
  (println "Opening default audio out: " (get-device-doc device))
  (apply ucs (append (list "open" "device" "out~" "__defaultAudioOut" "as") (get-device device)))
  (ucs "default" "out~" "__defaultAudioOut"))

;;
;; open-named-default-audio-out
;;
(define (open-named-default-audio-out name device)
  (println "Opening audio out " name "(also default): " (get-device-doc device))
  (apply ucs (append (list "open" "device" "out~" name "as") (get-device device)))
  (ucs "default" "out~" name))

;;
;; reset-midi
;;
(define (reset-midi)
  (ucs "reset" "device" "midi"))

;;
;; open-midi
;;
(define (open-midi num device)
  (println "Opening midi port " num ": " (get-device-doc device))
  (apply ucs (append (list "open" "device" "midi" num "as") (get-device device))))

;;
;; open-default-midi
;;
(define (open-default-midi device)
  (println "Opening default midi port: " (get-device-doc device))
  (apply ucs (append (list "open" "device" "midi" "0" "as") (get-device device))))

;;
;; Builtin  devices
;; 

; FIXME
(define-device "profile" "prof_dac channels 2" "Profile Dac")
