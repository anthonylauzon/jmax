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
;;  jMax system defaults of the system properties
;;  

;; The following list defines all the jMax boot variables.
;; See the rest of the file for their default values.
;;
;;     jmax-sample-rate
;;     jmax-audio-buffer
;;     jmax-connection
;;     jmax-mode
;;     jmax-server-name
;;     jmax-port
;;     jmax-pkg-path
;;     jmax-arch
;;     jmax-fast-file-box
;;     jmax-host
;;     jmax-host-type
;;     jmax-no-console
;;     jmax-no-register
;;     jmax-splash-screen
;;

;; default audio parameters

(define jmax-sample-rate 44100)
(define jmax-audio-buffer 2048)

;; set jMax client/server connection mode

(define jmax-connection "udp")

;; set binary execution mode for jMax server executable

(define jmax-mode "debug")

;; set name for jMax server executable

(define jmax-server-name "fts")

;; set port number needed for socket connection

(define jmax-port "2000")

;; default user package path

(define jmax-pkg-path (string-append (get-property "user.home") slash "jmax"))

;; default architecture if not specified for host name

(define jmax-arch "linuxpc")

;; default "UseIcons" value for Swing File Box

(define jmax-fast-file-box "false")

;; default host 

(define jmax-host "local")

;; default host type

(define jmax-host-type "linuxpc")

;; By default, use the jmax Console

(define jmax-no-console "false")

;; By default, show the register panel

(define jmax-no-register "false")

;; Splash screen setting

(define jmax-splash-screen "show")





