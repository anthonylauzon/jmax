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

(define jmax-mode "opt")

;; set name for jMax server executable

(define jmax-server-name "fts")

;; set port number needed for socket connection

(define jmax-port "2000")

;; default user package path

(define jmax-pkg-path (string-append (get-property "user.home") slash "jmax"))

;; default system package path
;;(define jmax-sys-pkg-path (string-append (string-append jmax-root slash "packages")
;;					 " "
;;					 (string-append jmax-root slash "syspackages")))
(append-package-path (string-append jmax-root slash "packages"))
(append-package-path (string-append jmax-root slash "syspackages"))

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


;; Compatibility mode

(define jmax-compatibility "ispw")




