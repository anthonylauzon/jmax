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

(define max-application-add-hook 
  (method "addHook" "ircam.jmax.MaxApplication" "java.lang.String" "ircam.jmax.script.Script"))

(define fts-help-patch-table-add 
  (method "add" "ircam.jmax.fts.FtsHelpPatchTable" "java.lang.String" "java.lang.String"))

(define fts-help-patch-table-add-summary 
  (method "addSummary" "ircam.jmax.fts.FtsHelpPatchTable" "java.lang.String" "java.lang.String"))

(define interpreter-add-script-menu 
  (method "addScriptMenu" "ircam.jmax.script.Interpreter" "java.lang.String" "ircam.jmax.script.Script" "java.lang.String" "java.lang.Object"))

(define interpreter-ask 
  (method "ask" "ircam.jmax.script.Interpreter" "java.lang.String" "java.lang.String"))

(define interpreter-convert 
  (method "convert" "ircam.jmax.script.Interpreter" "java.lang.Object"))

(define scheme-interpreter-get-key-stroke 
  (method "getKeyStroke" "ircam.jmax.script.scm.SchemeInterpreter" "java.lang.String" "boolean" "boolean" "boolean" "boolean"))

(define scheme-interpreter-load-silently 
  (method "loadSilently" "ircam.jmax.script.scm.SchemeInterpreter" "java.lang.String"))

(define add-pop-up-add-abbreviation 
  (method "addAbbreviation" "ircam.jmax.editors.patcher.AddPopUp" "java.lang.String" "java.lang.String" "java.lang.String" "boolean"))

(define add-pop-up-add-abbreviation1 
  (method "addAbbreviation" "ircam.jmax.editors.patcher.AddPopUp" "java.lang.String" "java.lang.String" "java.lang.String" "java.lang.String" "boolean"))

(define icons-load-icon 
  (method "loadIcon" "ircam.jmax.toolkit.Icons" "java.lang.String" "java.lang.String"))


;;
;; help-patch
;;
(define help-patch fts-help-patch-table-add)


;;
;; help-summary
;;
(define help-summary fts-help-patch-table-add-summary)

;;
;; declare-icon
;;
(define declare-icon icons-load-icon)

;;
;; println
;;
(define println 
  (lambda l (lprintln l)))

(define (lprintln l) 
  (if (null? l) 
      (newline) 
      (begin 
	(display (car l)) 
	(lprintln (cdr l)))))

;;
;; file-cat
;;
(define file-cat 
  (lambda l (lfile-cat (car l) (cdr l))))

(define (lfile-cat s l) 
  (if (null? l) 
      s
      (begin 
	(lfile-cat (string-append s slash (car l)) (cdr l)))))

;;
;; silent-load
;;
;; Load a file silently: if an error occurs, print out a message and
;; return, but do not stop the evaluation of the current file.
;;
(define (load-silently file)
  (scheme-interpreter-load-silently interpreter file))

;;
;; key-stroke
;;
;; Returns a Java KeyStroke object representing the requested key
;; stroke. "key" should be a string, the remaining arguments are
;; booleans specifying whether or not the modifier keys should be
;; pressed.
;;
(define (key-stroke key alt ctrl meta shift)
  (scheme-interpreter-get-key-stroke interpreter key alt ctrl meta shift))

;;
;; add-script-menu
;;
;; Adds a new entry in the script menu. "type" is the document type
;; the script handles. Use "all" for all type of documents. "name" is
;; the user visible menu name. "key" is the key stroke to which this
;; script is bound. Use the "key-stroke" procedure to construct the
;; correct key stroke. "script" detemines the script to be
;; executed. It can be a string expression or a procedure (Kawa only).
;;
(define (add-script-menu type script name key)
  (interpreter-add-script-menu interpreter type (make-script script ()) name key))

;;
;; ask
;; 
;; Ask the user to enter a value in a dialog. "question" is the string
;; to be displayed as title in the dialog. "return-type" is the type
;; of the answer. Valid types are "boolean", "int", "float", "string",
;; "symbol". 
;;
;; Ex. (println (ask "How many iterations?" "int") " iterations")
;;
(define (ask question return-type) 
  (interpreter-ask interpreter question return-type))

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
  (interpreter-convert interpreter
		       (if (null? arg)
			   proc
			   (lambda () (apply proc arg)))))


;;
;; define-hook
;;
;; "define" takes a label and a procedure. The procedure is either a
;; string expression or a Scheme procedure. In the latter case an
;; argument list can be given.
;;
(define define-hook
  (lambda (label proc . arg)
    (max-application-add-hook label (make-script proc arg))))


;;
;; patcher-menu
;;
;; (patcher-menu "add" null <name> <description> <message>)
;; (patcher-menu "add" <submenus> <name> <description> <message>)
;; (patcher-menu "addAndEdit" null <name> <description> <message>)
;; (patcher-menu "addAndEdit" <submenus> <name> <description> <message>)
;;
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

