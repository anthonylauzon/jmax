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

;; package declaration
(provide-package "dsdev" "0.0.0")

; load dynamic libraries into the server
(ucs "load" "module" "dsdev" (file-cat dir "c" "lib" (libname "dsdev")))
;(help-summary "DsDev summary" (file-cat dir "help" "dsdev.summary.jmax"))
;(help-patch "dsaudioport" (file-cat dir "help" "dsaudioport.help.jmax"))

(println "package: dsdev (DirectSound audioport)")