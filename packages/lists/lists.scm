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
(provide-package "lists" "0.0.0")

; load the dynamic libraries into the server
(ucs "load" "module" "lists" (file-cat dir "lib" (libname "lists")))

; require ISPW classes for the templates
(require-package "ispw" "0.0.0")

;; declare templates
(template "listcompose" (file-cat dir "templates" "listcompose.jmax"))
(template "listdecompose" (file-cat dir "templates" "listdecompose.jmax"))
(template "lcomp" (file-cat dir "templates" "listcompose.jmax"))
(template "ldeco" (file-cat dir "templates" "listdecompose.jmax"))

;; load the help patch data base
(sshh-load (file-cat dir "help" "lists.help.index.scm"))

(println "package: lists (list classes)")
