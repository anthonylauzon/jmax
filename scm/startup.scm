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
(ucs "show" "welcome")
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
(println "Running start hooks.")
(run-hooks "start")

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




