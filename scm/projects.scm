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
  (ucs "set" "param" "sample_rate" rate))

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

;;
;;  default-audio
;;
(define (default-audio . l)
  (display "Setting default audio port to ") 
  (println l)
  (apply ucs (append (list "newobj" "DefaultAudioPort" ":") l)))


;;
;;  default-midi
;;
(define (default-midi . l)
  (display "Setting default midi to ") 
  (println l)
  (apply ucs (append (list "default" "midi") l)))
