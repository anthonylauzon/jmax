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
(if (not (java-null? (get-property "jmaxHostType")))    (set! jmax-host-type (get-property "jmaxHostType")))

;; the following is done so properties it can be accessed from Java
;;

(set-property "jmaxFastFileBox" jmax-fast-file-box)
(set-property "jmaxNoConsole" jmax-no-console)
(set-property "jmaxNoRegister" jmax-no-register)


