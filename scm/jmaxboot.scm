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

;;
;; Load the intrinsic Scheme functions. 
;;
(load (string-append jmax-scm-root slash "scheme_interface.scm"))

;;
;; FIXME testing only
;;
(define start-time (system-current-time-millis))

;;
;; Intrinsic functions for the Sgi platform
;;
;(load (file-cat jmax-scm-root "devices.scm"))

;;
;; Tcl functions to handle projects primitives
;;
(load (file-cat jmax-scm-root "projects.scm"))

;;
;; Set parameters default values
;;
(load (file-cat jmax-scm-root "defaults.scm"))

;;
;; Pass the system package path to the package handler
;;
;;(append-package-path jmax-sys-pkg-path)

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

