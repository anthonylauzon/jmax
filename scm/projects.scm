;
;  jMax
;  
;  Copyright (C) 1999 by IRCAM
;  All rights reserved.
;  
;  Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;
;  This program may be used and distributed under the terms of the 
;  accompanying LICENSE.
; 
;  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
;  for DISCLAIMER OF WARRANTY.
;  

;;
;;  Configuration commands (for project.env and .jmaxrc)
;;  

;; abstraction-directory checks if in the directory given as argument
;; there is a abstractions.scm file, and load it if there; otherwise
;; it add the given directory to the abstraction search path using
;; the abstractionPath command
;;
(define (abstraction-directory dir)
  (let ((file-name (file-cat dir "template.scm")))
    (if (file-exists (new-file file-name))
	(sshh-load file-name)
	(abstraction-path dir))))
;;
;; Same thing for templates
;;
(define (template-directory dir)
  (let ((file-name (file-cat dir "template.scm")))
    (if (file-exists (new-file file-name))
	(sshh-load file-name)
	(abstraction-path dir))))

;;
;; project-directory
;;
(define (project-directory dir)
  (ucs "set" "projectdir" dir))

;;
;; The data directory string
;;
(define data-directory-string "")

;;
;; data-directory
;;
(define (data-directory dir)
  (set! data-directory-string (string-append data-directory-string ":" dir))
  (ucs "set" "defaultpath" data-directory-string))

;;
;; Local copy of the sample rate
;;
(define jmax-sample-rate 44100)

;;
;; jmax-set-sample-rate
;;
(define (jmax-set-sample-rate rate)
  (set! jmax-sample-rate rate)
  (ucs "set" "param" "sampling_rate" (number->string rate)))

;;
;; get-sample-rate
;;
(define (get-sample-rate) jmax-sample-rate)

;;
;; Local copy of audio buffer size
;;
(define jmax-audio-buffer 2048)

;;
;; jmax-set-audio-buffer
;;
(define (jmax-set-audio-buffer size)
  (set! jmax-audio-buffer size)
  (ucs "set" "param" "fifo_size" (number->string size)))

;;
;;  get-audio-buffer
;;
(define (get-audio-buffer) jmax-audio-buffer)





