(define (max-application-get-fts)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "getFts" <ircam.jmax.fts.Fts> ())
  ))

(define (max-application-set-fts a0)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "setFts" <void> (<ircam.jmax.fts.Fts>))
   a0))

(define (max-application-get-property a0)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "getProperty" <java.lang.String> (<java.lang.String>))
   a0))

(define (max-application-set-property a0 a1)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "setProperty" <java.lang.String> (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (max-application-add-hook a0 a1)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "addHook" <void> (<java.lang.String> <ircam.jmax.script.Script>))
   a0 a1))

(define (max-application-run-hooks a0)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "runHooks" <void> (<java.lang.String>))
   a0))

(define (max-application-quit)
  ((primitive-static-method
    <ircam.jmax.MaxApplication>
    "Quit" <void> ())
  ))

(define (max-application-install-editor-factory a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.MaxApplication>
    "installEditorFactory" <void> (<ircam.jmax.mda.MaxDataEditorFactory>))
   a0 a1))

(define (max-application-install-document-handler a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.MaxApplication>
    "installDocumentHandler" <void> (<ircam.jmax.mda.MaxDocumentHandler>))
   a0 a1))

(define (max-application-install-document-type a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.MaxApplication>
    "installDocumentType" <void> (<ircam.jmax.mda.MaxDocumentType>))
   a0 a1))

(define (interpreter-convert a0 a1)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "convert" <ircam.jmax.script.Script> (<java.lang.Object>))
   a0 a1))

(define (interpreter-get-package-handler a0)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "getPackageHandler" <ircam.jmax.script.pkg.PackageHandler> ())
   a0))

(define (interpreter-load-package a0 a1 a2)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "loadPackage" <ircam.jmax.script.pkg.Package> (<ircam.jmax.script.pkg.Package> <java.io.File>))
   a0 a1 a2))

(define (interpreter-load-project a0 a1 a2)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "loadProject" <ircam.jmax.script.pkg.Project> (<ircam.jmax.script.pkg.Package> <java.io.File>))
   a0 a1 a2))

(define (interpreter-load-scripted-document a0 a1 a2)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "loadScriptedDocument" <ircam.jmax.mda.MaxDocument> (<ircam.jmax.script.pkg.Package> <java.io.File>))
   a0 a1 a2))

(define (interpreter-add-script-menu-item a0 a1 a2 a3 a4)
  ((primitive-interface-method
    <ircam.jmax.script.Interpreter>
    "addScriptMenuItem" <void> (<java.lang.String> <java.lang.String> <java.lang.Object> <ircam.jmax.script.Script>))
   a0 a1 a2 a3 a4))

(define (scheme-interpreter-get-key-stroke a0 a1 a2 a3 a4 a5)
  ((primitive-virtual-method
    <ircam.jmax.script.scm.SchemeInterpreter>
    "getKeyStroke" <javax.swing.KeyStroke> (<java.lang.String> <boolean> <boolean> <boolean> <boolean>))
   a0 a1 a2 a3 a4 a5))

(define (scheme-interpreter-load-silently a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.scm.SchemeInterpreter>
    "loadSilently" <java.lang.Object> (<java.lang.String>))
   a0 a1))

(define (scheme-interpreter-to-java-string a0)
  ((primitive-static-method
    <ircam.jmax.script.scm.SchemeInterpreter>
    "toJavaString" <java.lang.Object> (<java.lang.Object>))
   a0))

(define (scheme-interpreter-is-null a0)
  ((primitive-static-method
    <ircam.jmax.script.scm.SchemeInterpreter>
    "isNull" <boolean> (<java.lang.Object>))
   a0))

(define (package-handler-set-unknown-script a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "setUnknownScript" <void> (<ircam.jmax.script.Script>))
   a0 a1))

(define (package-handler-get-unknown-script a0)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "getUnknownScript" <ircam.jmax.script.Script> ())
   a0))

(define (package-handler-provide a0 a1 a2)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "provide" <void> (<java.lang.String> <java.lang.String>))
   a0 a1 a2))

(define (package-handler-forget a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "forget" <void> (<java.lang.String>))
   a0 a1))

(define (package-handler-require a0 a1 a2)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "require" <void> (<java.lang.String> <java.lang.String>))
   a0 a1 a2))

(define (package-handler-append-path a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.PackageHandler>
    "appendPath" <void> (<java.lang.String>))
   a0 a1))

(define (package-load-class a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.Package>
    "loadClass" <java.lang.Class> (<java.lang.String>))
   a0 a1))

(define (package-append-local-path a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.script.pkg.Package>
    "appendLocalPath" <void> (<java.lang.String>))
   a0 a1))

(define (new-fts a0 a1 a2 a3 a4)
  ((primitive-constructor
    <ircam.jmax.fts.Fts>
    (<java.lang.String> <java.lang.String> <java.lang.String> <java.lang.String> <java.lang.String>))
   a0 a1 a2 a3 a4))

(define (fts-make-fts-object a0 a1 a2 a3)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "makeFtsObject" <ircam.jmax.fts.FtsObject> (<ircam.jmax.fts.FtsObject> <java.lang.String> <java.lang.String>))
   a0 a1 a2 a3))

(define (fts-make-fts-connection a0 a1 a2 a3 a4)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "makeFtsConnection" <ircam.jmax.fts.FtsConnection> (<ircam.jmax.fts.FtsObject> <int> <ircam.jmax.fts.FtsObject> <int>))
   a0 a1 a2 a3 a4))

(define (fts-redefine-fts-object a0 a1 a2)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "redefineFtsObject" <ircam.jmax.fts.FtsObject> (<ircam.jmax.fts.FtsObject> <java.lang.String>))
   a0 a1 a2))

(define (fts-get-selection a0)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "getSelection" <ircam.jmax.fts.FtsSelection> ())
   a0))

(define (fts-sync a0)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "sync" <void> ())
   a0))

(define (fts-stop a0)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "stop" <void> ())
   a0))

(define (fts-abstraction-declare a0 a1 a2)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "abstractionDeclare" <void> (<java.lang.String> <java.lang.String>))
   a0 a1 a2))

(define (fts-abstraction-path-declare a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "abstractionPathDeclare" <void> (<java.lang.String>))
   a0 a1))

(define (fts-template-declare a0 a1 a2)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "templateDeclare" <void> (<java.lang.String> <java.lang.String>))
   a0 a1 a2))

(define (fts-template-path-declare a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "templatePathDeclare" <void> (<java.lang.String>))
   a0 a1))

(define (fts-ucs-command a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.fts.Fts>
    "ucsCommand" <void> (<ircam.jmax.utils.MaxVector>))
   a0 a1))

(define (fts-help-patch-table-add a0 a1)
  ((primitive-static-method
    <ircam.jmax.fts.FtsHelpPatchTable>
    "add" <void> (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (fts-help-patch-table-add-summary a0 a1)
  ((primitive-static-method
    <ircam.jmax.fts.FtsHelpPatchTable>
    "addSummary" <void> (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (fts-reference-u-r-l-table-add a0 a1)
  ((primitive-static-method
    <ircam.jmax.fts.FtsReferenceURLTable>
    "add" <void> (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (mda-install-editor-factory a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "installEditorFactory" <void> (<ircam.jmax.mda.MaxDataEditorFactory>))
   a0))

(define (mda-edit a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "edit" <ircam.jmax.mda.MaxDataEditor> (<ircam.jmax.mda.MaxData>))
   a0))

(define (mda-install-document-handler a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "installDocumentHandler" <void> (<ircam.jmax.mda.MaxDocumentHandler>))
   a0))

(define (mda-can-load-document a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "canLoadDocument" <boolean> (<java.io.File>))
   a0))

(define (mda-load-document a0 a1)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "loadDocument" <ircam.jmax.mda.MaxDocument> (<ircam.jmax.mda.MaxContext> <java.io.File>))
   a0 a1))

(define (mda-find-document-handler-for a0 a1)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "findDocumentHandlerFor" <ircam.jmax.mda.MaxDocumentHandler> (<java.io.File> <ircam.jmax.mda.MaxDocument>))
   a0 a1))

(define (mda-install-document-type a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "installDocumentType" <void> (<ircam.jmax.mda.MaxDocumentType>))
   a0))

(define (mda-new-document a0)
  ((primitive-static-method
    <ircam.jmax.mda.Mda>
    "newDocument" <ircam.jmax.mda.MaxDocument> (<ircam.jmax.mda.MaxData>))
   a0))

(define (max-document-get-name a0)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "getName" <java.lang.String> ())
   a0))

(define (max-document-set-name a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "setName" <void> (<java.lang.String>))
   a0 a1))

(define (max-document-edit a0)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "edit" <ircam.jmax.mda.MaxDataEditor> ())
   a0))

(define (max-document-save a0)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "save" <void> ())
   a0))

(define (max-document-save-to a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "saveTo" <void> (<java.io.File>))
   a0 a1))

(define (max-document-dispose a0)
  ((primitive-virtual-method
    <ircam.jmax.mda.MaxDocument>
    "dispose" <void> ())
   a0))

(define (icons-load-icon a0 a1)
  ((primitive-static-method
    <ircam.jmax.toolkit.Icons>
    "loadIcon" <void> (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (new-max-vector)
  ((primitive-constructor
    <ircam.jmax.utils.MaxVector>
    ())
  ))

(define (max-vector-add-element a0 a1)
  ((primitive-virtual-method
    <ircam.jmax.utils.MaxVector>
    "addElement" <void> (<java.lang.Object>))
   a0 a1))

(define (add-pop-up-add-abbreviation a0 a1 a2 a3)
  ((primitive-static-method
    <ircam.jmax.editors.patcher.AddPopUp>
    "addAbbreviation" <void> (<java.lang.String> <java.lang.String> <java.lang.String> <boolean>))
   a0 a1 a2 a3))

(define (add-pop-up-add-abbreviation1 a0 a1 a2 a3 a4)
  ((primitive-static-method
    <ircam.jmax.editors.patcher.AddPopUp>
    "addAbbreviation" <void> (<java.lang.String> <java.lang.String> <java.lang.String> <java.lang.String> <boolean>))
   a0 a1 a2 a3 a4))

(define (new-console-window)
  ((primitive-constructor
    <ircam.jmax.editors.console.ConsoleWindow>
    ())
  ))

(define (new-splash-dialog a0 a1)
  ((primitive-constructor
    <ircam.jmax.dialogs.SplashDialog>
    (<java.lang.String> <java.lang.String>))
   a0 a1))

(define (system-current-time-millis)
  ((primitive-static-method
    <java.lang.System>
    "currentTimeMillis" <long> ())
  ))

(define (system-get-properties)
  ((primitive-static-method
    <java.lang.System>
    "getProperties" <java.util.Properties> ())
  ))

(define (object-get-class a0)
  ((primitive-virtual-method
    <java.lang.Object>
    "getClass" <java.lang.Class> ())
   a0))

(define (object-to-string a0)
  ((primitive-virtual-method
    <java.lang.Object>
    "toString" <java.lang.String> ())
   a0))

(define (new-string2 a0)
  ((primitive-constructor
    <java.lang.String>
    (<[C>))
   a0))

(define (string-length a0)
  ((primitive-virtual-method
    <java.lang.String>
    "length" <int> ())
   a0))

(define (number-int-value a0)
  ((primitive-virtual-method
    <java.lang.Number>
    "intValue" <int> ())
   a0))

(define (number-float-value a0)
  ((primitive-virtual-method
    <java.lang.Number>
    "floatValue" <float> ())
   a0))

(define (new-float a0)
  ((primitive-constructor
    <java.lang.Float>
    (<float>))
   a0))

(define (new-integer a0)
  ((primitive-constructor
    <java.lang.Integer>
    (<int>))
   a0))

(define (class-for-name a0)
  ((primitive-static-method
    <java.lang.Class>
    "forName" <java.lang.Class> (<java.lang.String>))
   a0))

(define (new-file a0)
  ((primitive-constructor
    <java.io.File>
    (<java.lang.String>))
   a0))

(define (file-exists a0)
  ((primitive-virtual-method
    <java.io.File>
    "exists" <boolean> ())
   a0))

(define (hashtable-get a0 a1)
  ((primitive-virtual-method
    <java.util.Hashtable>
    "get" <java.lang.Object> (<java.lang.Object>))
   a0 a1))

(define (hashtable-put a0 a1 a2)
  ((primitive-virtual-method
    <java.util.Hashtable>
    "put" <java.lang.Object> (<java.lang.Object> <java.lang.Object>))
   a0 a1 a2))

