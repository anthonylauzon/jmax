EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
Actions.java\
AlignBottomAction.java\
AlignLeftAction.java\
AlignRightAction.java\
AlignTopAction.java\
BringToFrontAction.java\
BringToFrontObjectAction.java\
CopyAction.java\
CutAction.java\
DeleteSelectionAction.java\
DuplicateAction.java\
FindAction.java\
FindErrorsAction.java\
FitToTextAction.java\
FitToTextPopUpAction.java\
FontAction.java\
FontBiggerAction.java\
FontBiggerPopUpAction.java\
FontPopUpAction.java\
FontSizesAction.java\
FontSizesPopUpAction.java\
FontSmallerAction.java\
FontSmallerPopUpAction.java\
FontStylesAction.java\
FontStylesPopUpAction.java\
InspectAction.java\
JMaxFontAction.java\
JMaxFontPopUpAction.java\
JMaxFontSizesAction.java\
JMaxFontSizesPopUpAction.java\
LockAction.java\
MoveAction.java\
OpenHelpPatchAction.java\
PasteAction.java\
PrintAction.java\
RemoveInOutletConnection.java\
ResizeAction.java\
ResizeToMaxHeightAction.java\
ResizeToMaxWidthAction.java\
SaveAction.java\
SaveAsAction.java\
SaveToAction.java\
ScalePatcherAction.java\
SelectAllAction.java\
SendToBackAction.java\
SendToBackObjectAction.java\
SetAutomaticFitAction.java\
ShowErrorAction.java\
ShowHTMLAction.java

JAVACX=@JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES= .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~


