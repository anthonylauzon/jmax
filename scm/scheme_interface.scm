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

;; Scheme interface
;;
;; The files kawa_interface and kawa_interface both define the basic
;; functions to access jMax objects. This file contains an additional
;; set of functions to access jMax. In general, they have more "user
;; friendly" names and provide an easier to use syntax. These functions
;; are independent of the interpreter.
;; 

; FIXME
;("sourceFile", new MaxSourceCmd());
;(define (jmax ...  new JMaxCmd());

;;
;; The following functions are just a short cut to their longer-named
;; equivalents in {kawa,silk}_interface.scm.
;;
(define get-fts                      max-application-get-fts)
(define help-patch                   fts-help-patch-table-add)
(define help-summary                 fts-help-patch-table-add-summary)
(define reference-url                fts-reference-u-r-l-table-add)
(define splash                       new-splash-dialog)
(define declare-icon                 icons-load-icon)
(define run-hooks                    max-application-run-hooks)
(define get-property                 max-application-get-property)
(define set-property                 max-application-set-property)
(define quit                         max-application-quit)
(define open-console                 new-console-window)
(define exit                         max-application-quit)

;;
;; Some handy functions:
;;
;;   warning: Prints out a warning message followed by all the
;;   arguments.
;;
;;   println: Prints all the arguments on the output followed by a
;;   new-line.
;;
;;   cat: Concatenates all the strings given as argument (=
;;   string-append).
;;
;;   file-cat: Concatenates all the strings given as argument, putting a
;;   file separator in between.
;;
(define warning 
  (lambda l 
    (begin (display "jMax: Warning: ") 
	   (lprintln l))))

(define println 
  (lambda l (lprintln l)))

(define (lprintln l) 
  (if (null? l) 
      (newline) 
      (begin 
	(display (car l)) 
	(lprintln (cdr l)))))

(define cat string-append)

(define file-cat 
  (lambda l (lfile-cat (car l) (cdr l))))

(define (lfile-cat s l) 
  (if (null? l) 
      s
      (begin 
	(lfile-cat (string-append s slash (car l)) (cdr l)))))

;;
;; A default implementation of the browse command
;; so that the kernel code can call it
;;
(define (browse location) 
  (warning "No Browser installed !!!"))

;;
;; The basic help command, just call up the browser 
;; with a generic help html text.
;;
(define (help)
  (browse (cat "file:" jmax-root slash "doc" slash "help.html")))

;;
;; Define an host type
;;
(define jmax-host-type-list ())

(define (define-host name type)
  (set! jmax-host-type-list (cons (list name type) jmax-host-type-list)))

(define (get-host-type name)
  (get-host-type-recur name jmax-host-type-list))

(define (get-host-type-recur name list)
  (if (null? list)
      ()
      (if (string=? name (caar list))
	  (cadar list)
	  (get-host-type-recur name (cdr list)))))

;;
;; sshh-load
;;
;; Load a file silently: if an error occurs, print out a message and
;; return, but do not stop the evaluation of the current file.
;;
(define (sshh-load file)
  (scheme-interpreter-load-silently jmax-interp file))

;;
;; key-stroke
;;
;; Returns a Java KeyStroke object representing the requested key
;; stroke. "key" should be a string, the remaining arguments are
;; booleans specifying whether or not the modifier keys should be
;; pressed.
;;
(define (key-stroke key alt ctrl meta shift)
  (scheme-interpreter-get-key-stroke jmax-interp key alt ctrl meta shift))

;;
;; script-menu
;;
;; Adds a new entry in the script menu for the given document
;; type. Document type "all" signifies that the menu will be visible
;; in all document menu bars. "name" is the user visible menu
;; name. "key" is the key stroke to which this script is bound. Use
;; the "key-stroke" procedure to construct the correct key
;; stroke. "script" detemines the script to be executed. It can be a
;; string expression or a procedure (Kawa only).
;;
(define script-menu
  (lambda (doc name key script . arg)
    (interpreter-add-script-menu-item jmax-interp doc name key (make-script script arg))))

;;
;; make-script
;;
;; Converts a procedure with arguments into an interpreter script. If
;; the procedure is a string it is taken to be an expression that will
;; be evaluated. Otherwise proc is considered to be a procedure. If
;; that argument list is not empty (only allowed when proc is a
;; procedure) they are enclosed in a lambda expression.  Silk
;; currently does not support to pass Scheme procedures as argument.
;;
(define (make-script proc arg)
  (interpreter-convert jmax-interp
		       (if (null? arg)
			   proc
			   (lambda () (apply proc arg)))))

;;
;; java-null?
;;
;; The Java null value is not represented in Scheme. Silk and Kawa
;; handle it differently. This procedure is provided to test whether
;; an Object is null or not.
;; 
(define java-null? scheme-interpreter-is-null)

;;
;; to-java-string
;;
;; A Scheme string passed as argument to a Java methods that expects a
;; Java string is automatically coerced. In some cases, however, the
;; coerced must be done explicitely. This is the case when one wants
;; to put a string in a hashtable, for example. Since the hashtable
;; expects an Object, no coercion is done.
;;
(define to-java-string scheme-interpreter-to-java-string)

;;
;; list->max-vector
;;
(define (list->max-vector l)
  (max-vector-add-list (new-max-vector) l))

(define (max-vector-add-list v l)
  (if (null? l)
      v
      (begin
	(if (string? (car l))
	    (max-vector-add-element v (to-java-string (car l)))
	    (max-vector-add-element v (car l)))
	(max-vector-add-list v (cdr l)))))

;;
;; set-system-property
;;
;; The difference with set-property is that here we immediately set a
;; value in the system properties while set-property sets a value in the
;; scriptable applications properties. 
;;
(define (set-system-property key value)
  (hashtable-put (system-get-properties) (to-java-string key) (to-java-string value)))

;;
;; get-system-property
;;
;; There exists two getProperty methods in the Property class. To
;; avoid conflicts, use the Hashtable.get.
;;
(define (get-system-property key)
  (hashtable-get (system-get-properties) (to-java-string key)))

;;
;; get-package-handler
;;
(define (get-package-handler)
  (interpreter-get-package-handler jmax-interp))

;;
;; append-local-path
;;
;; Append a path to the package's local path list.
;;
(define (append-local-path pkg path)
  (package-append-local-path pkg path))

;;
;; load-class
;;
(define (load-class pkg claz)
  (package-load-class pkg claz))

;;
;; set-package-unknown-script
;;
(define (set-package-unknown-script script)
  (package-handler-set-unknown-script (get-package-handler) script))

;;
;; get-package-unknown-script
;;
(define (get-package-unknown-script)
  (package-handler-get-unknown-script (get-package-handler)))

;;
;; provide-package
;;
(define (provide-package name version)
  (package-handler-provide (get-package-handler) name version))

;;
;; forget-package
;;
(define (forget-package name)
  (package-handler-forget (get-package-handler) name))

;;
;; require-package
;;
(define (require-package name version)
  (package-handler-require (get-package-handler) name version))

;;
;; append-package-path
;;
(define (append-package-path path)
  (package-handler-append-path (get-package-handler) path))

;;
;; patcher-menu
;;
;; (patcher-menu "add" null <name> <description> <message>)
;; (patcher-menu "add" <submenus> <name> <description> <message>)
;; (patcher-menu "addAndEdit" null <name> <description> <message>)
;; (patcher-menu "addAndEdit" <submenus> <name> <description> <message>)
;;

; Some strange bug in Silk. If I use (string=?  menu "add") instead of
; (string=? "add" menu), the rest of the procedure and the rest of the
; file patcher_menu.scm is not evaluated without error or warning.
(define (patcher-menu-test menu sub name descr message)
  (println "Patcher Menu")
  (println menu)
  (if (string=? "add" menu) (println #t) (println #f)))

(define (patcher-menu menu sub name descr message)
  (if (string=? "add" menu)
      (if (null? sub)
	  (add-pop-up-add-abbreviation name descr message #f) 
	  (add-pop-up-add-abbreviation1 sub name descr message #f))
      (if (string=? "addAndEdit" menu)
	  (if (null? sub)
	      (add-pop-up-add-abbreviation name descr message #t) 
	      (add-pop-up-add-abbreviation1 sub name descr message #f))
	  (warning "Unknown menu type: " menu))))

;;
;; abstraction
;;
(define (abstraction name filename)
  (fts-abstraction-declare (get-fts) name filename))

;;
;; abstraction-path
;;
(define (abstraction-path path)
  (fts-abstraction-path-declare (get-fts) path))

;;
;; template
;;
(define (template name filename)
  (fts-template-declare (get-fts) name filename))

;;
;; template-path
;;
(define (template-path path) 
  (fts-template-path-declare (get-fts) path))

;;
;; ucs
;;
(define ucs 
  (lambda arg (fts-ucs-command (get-fts) (list->max-vector arg))))

;;
;; sync
;;
(define (sync)
  (let ((before (system-current-time-millis)))
    (fts-sync (get-fts))
    (- (system-current-time-millis) before)))

;;
;; fts-connect
;;
(define (fts-connect dir name type server port)
  (max-application-set-fts (new-fts dir name type server port)))

;;
;; open
;;
(define (open file)
  (if (string=? (substring file 0 1) "~")
      (open (cat (get-system-property "user.home") 
		 (substring file 1 (string-length file))))
      (max-document-edit (mda-load-document (get-fts) (new-file file)))))

;;
;; get-max-version
;;
(define (get-max-version) 
  (get-property "jmaxVersion")) 

;;
;; when
;;
;; "when" takes a label and a procedure. The procedure is either a
;; string expression or a Scheme procedure. In the latter case an
;; argument list can be given.
;;
(define when
  (lambda (label proc . arg)
    (max-application-add-hook label (make-script proc arg))))


