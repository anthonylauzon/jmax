;; 
;; jMax
;; Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
;; 
;; This program is free software; you can redistribute it and/or
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
;; Based on Max/ISPW by Miller Puckette.
;;
;; Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
;;

(provide-package "data" "0.0.0")

(ucs "load" "module" "data" (file-cat dir "c" "lib" jmax-arch jmax-mode (libname "data")))

; load client java class
(append-local-path this-package (file-cat "java" "classes"))
(load-class this-package "ircam.jmax.editors.bpf.BpfExtension")
(load-class this-package "ircam.jmax.editors.table.TableExtension")

;; help
(sshh-load (file-cat dir "help" "data.help.index.scm"))
(help-summary "data summary" (file-cat dir "help" "data.summary.jmax"))

(println "package: data (data structures)")

