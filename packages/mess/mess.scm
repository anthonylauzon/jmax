;;
;;  jMax
;;  
;;  Copyright (C) 1999 by IRCAM
;;  All rights reserved.
;;  
;;  This program may be used and distributed under the terms of the 
;;  accompanying LICENSE.
;; 
;;  This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
;;  for DISCLAIMER OF WARRANTY.
;;  
(provide-package "mess" "0.0.0")

(ucs "load" "module" "mess" (file-cat dir "c" "lib" (libname "mess")))

(println "package: mess (message classes)")

;; help
(sshh-load (file-cat dir "help" "mess.help.index.scm"))
(help-summary "mess summary" (file-cat dir "help" "mess.summary.jmax"))


