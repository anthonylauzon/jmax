; 
; jMax
; Copyright (C) 1999 by IRCAM
; 
; This program is free software; you can redistribute it and/or
; modify it under the terms of the GNU General Public License
; as published by the Free Software Foundation; either version 2
; of the License, or (at your option) any later version.
; 
; See file LICENSE for further informations on licensing terms.
; 
; This program is distributed in the hope that it will be useful,
; but WITHOUT ANY WARRANTY; without even the implied warranty of
; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
; GNU General Public License for more details.
; 
; You should have received a copy of the GNU General Public License
; along with this program; if not, write to the Free Software
; Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
; 
; Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;

;; package declaration
(provide-package "explode" "0.0.0")

; load server module
(ucs "load" "module" "explode" (file-cat dir "c" "lib" (libname "explode")))

; load client java class
(append-local-path this-package (file-cat "java" "classes"))
(load-class this-package "ircam.jmax.editors.explode.ExplodeExtension")

;; load the help patch data base
(sshh-load (file-cat dir "help" "explode.help.index.scm"))
(help-summary "explode summary" (file-cat dir "help" "explode.summary.jmax"))

(println "package: explode (ISPW sequencing class with editor)")



