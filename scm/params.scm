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
;; Process parameters in the command line 
;;

;; Get commandline argument

(if (not (java-null? (get-property "jmaxNoConsole"))) (set! jmax-no-console (get-property "jmaxNoConsole")))
(if (not (java-null? (get-property "jmaxNoRegister"))) (set! jmax-no-register (get-property "jmaxNoRegister")))
(if (not (java-null? (get-property "jmaxHost"))) (set! jmax-host (get-property "jmaxHost")))
(if (not (java-null? (get-property "jmaxConnection"))) (set! jmax-connection (get-property "jmaxConnection")))
(if (not (java-null? (get-property "jmaxPort"))) (set! jmax-port (get-property "jmaxPort")))
(if (not (java-null? (get-property "jmaxArch"))) (set! jmax-arch (get-property "jmaxArch")))
(if (not (java-null? (get-property "jmaxMode"))) (set! jmax-mode (get-property "jmaxMode")))
(if (not (java-null? (get-property "jmaxServerName"))) (set! jmax-server-name (get-property "jmaxServerName")))
(if (not (java-null? (get-property "jmaxPkgPath"))) (set! jmax-pkg-path (get-property "jmaxPkgPath")))
(if (not (java-null? (get-property "jmaxSplashScreen"))) (set! jmax-splash-screen (get-property "jmaxSplashScreen")))
(if (not (java-null? (get-property "jmaxFastFileBox"))) (set! jmax-fast-file-box (get-property "jmaxFastFileBox")))

;; the following is done so properties it can be accessed from Java
;;

(set-property "jmaxFastFileBox" jmax-fast-file-box)
(set-property "jmaxNoConsole" jmax-no-console)
(set-property "jmaxNoRegister" jmax-no-register)

;;
;; Special handling for host type
;;

; FIXME

(if (not (java-null? (get-property "jmaxHostType")))
    (set! jmax-host-type (get-property "jmaxHostType"))
    (if (string? (get-host-type jmax-host))
	(set! jmax-host-type (get-host-type jmax-host))
	(set! jmax-host-type "linuxpc")))

