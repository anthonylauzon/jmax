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

;; Jmaxboot.scm is the first Scheme file to be loaded
;;
;; it load all the intrinsic Scheme functions (i.e. the function
;; jmax *need* in order to work), the jmax package system,
;; and then the configuration file, that in turn should load
;; the packages.
;;
;; The interpreter sets the following vriable before loading this
;; file:
;;
;;    jmax-root: The path to jMax root directory
;;    jmax-interp: A reference to the interpreter object
;;    jmax-interp-name: The name of the interpreter {silk,kawa}
;;    slash: The system dependent file separator
;;

;;
;; System paths
;;
(define jmax-scm-root (string-append jmax-root slash "scm"))
(define jmax-image-root (string-append jmax-root slash "images" slash))
(define jmax-sys-pkg-path (string-append jmax-root slash "packages"))

;;
;; Load the interface functions specific to the Scheme
;; interpreter.
;;
(load (string-append jmax-scm-root slash jmax-interp-name "_interface.scm"))

;;
;; FIXME testing only
;;
(define start-time (system-current-time-millis))

;;
;; Load the intrinsic Scheme functions. 
;;
(load (string-append jmax-scm-root slash "scheme_interface.scm"))

;;
;; Pass the system package path to the package handler
;;
(append-package-path jmax-sys-pkg-path)

;;
;; Intrinsic functions for the Sgi platform
;;
(load (file-cat jmax-scm-root "devices.scm"))

;;
;; Tcl functions to handle projects primitives
;;
(load (file-cat jmax-scm-root "projects.scm"))

;;
;; Max packages 
;;
;(load (file-cat jmax-scm-root "jmaxpkg.scm"))

;;
;; Set parameters default values
;;
(load (file-cat jmax-scm-root "defaults.scm"))

;;
;; Load the system icons
;;
(load (file-cat jmax-scm-root "icons.scm"))

;;
;; Define the patcher editor popup menu
;; 
(load (file-cat jmax-scm-root "patcher_menu.scm"))

;;
;; Declare the 'virtual' jmax package
;;
(provide-package "jMax" "2.0.2")

;;
;; User Configuration
;;
(define usr-jmaxrc (file-cat (get-property "user.home") ".jmaxrc.scm"))
(if (file-exists (new-file usr-jmaxrc))
    (load usr-jmaxrc))

;;
;; Site specific configurations
;;
(load (file-cat jmax-root "config" "site.scm"))

;;
;; Load the machine file if existing
;; 
(define machine-file (file-cat jmax-root "config" jmax-host ".scm"))
(if (file-exists (new-file machine-file))
    (load machine-file))

;;
;; Process the parameters (i.e. consider defaults and command line arguments)
;; 
(load (file-cat jmax-scm-root "params.scm"))

;;
;; Load the machine type file if existing
;; 
(define machine-type-file1 (file-cat jmax-root "config" (cat jmax-host-type ".scm")))
(define machine-type-file2 (file-cat jmax-root "scm" "config" (cat jmax-host-type ".scm")))

(if (file-exists (new-file machine-type-file1))
    (load machine-type-file1)
    (if (file-exists (new-file machine-type-file2))
	(load machine-type-file2)
	(println "Didn't find a configuration file for host type " jmax-host-type)))

;;
;; do the real startup 
;;
(load (file-cat jmax-scm-root "startup.scm"))

;;
;; FIXME testing only
;;
(println (- (system-current-time-millis) start-time) " ms")

