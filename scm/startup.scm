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
;;  jMax startup file
;;

;;
;; Open the console
;;
(open-console)

;;
;; Run the official jMax splash screen if not supressed by the user in
;; .jmaxrc
;;
(if (not (string=? jmax-splash-screen "hide")) 
    (splash (file-cat jmax-root "images" "Splash.gif") (get-max-version)))

;;
;; Start jMax server
;;

(define jmax-server-dir 
  (file-cat jmax-root "fts" "bin" jmax-arch jmax-mode))

(if (not (java-null? (get-property "jmaxServerDir")))
    (set! jmax-server-dir (get-property "jmaxServerDir")))

(println jmax-server-dir)

(if (string=? jmax-connection "scheme")
    (println "jMax starting server on " jmax-host " via TCP/IP")
    (if (string=? jmax-connection "scheme")
	(println "jMax starting server on " jmax-host " via TCP/IP port " jmax-port)
	(if (string=? jmax-connection "udp")
	    (println "jMax starting server on " jmax-host " via UDP/IP"))))

(println "jMax Host type " jmax-host-type)

(if (string=? jmax-mode "debug")
    (println "jMax in DEBUG mode"))

(fts-connect jmax-server-dir jmax-server-name jmax-connection jmax-host jmax-port)

;;
;; hello server
;;
(sync)

;;
;; load gui server side objects
;;
(require-package "guiobj" "0.0.0")
(require-package "system" "0.0.0")

;;
;; Load installation default packages.
;; Use silent load (sshh-load) as a protection against user errors
;;
(sshh-load (file-cat jmax-root "config" "packages.scm"))

;;
;; Set sample rate and audio buffer.
;;
(jmax-set-sample-rate jmax-sample-rate)
(jmax-set-audio-buffer jmax-audio-buffer)

;;
;; Run the start Hooks, by hand so we are sure the correct
;; configuration is there ...
;;
(run-hooks "platform-start")

(if (not (run-hooks "start"))
    (run-hooks "default-start"))

;;
;; If profiling is on, close audio device 
;; and install the profile device; this after
;; the when start, i.e. including user configuration
;;
(if (and (not (java-null? (get-property "profile"))) 
	 (string=? (get-property "profile") "true"))
    (println "Running with pseudo audio device for profiling")
    (reset-audio-out)
    (reset-audio-in)
    (open-default-audio-out "profile"))




