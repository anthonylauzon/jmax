SUBDIRS= actions interactions menus objects

noinst_PROGRAMS = package
package_SOURCES = \
AddPopUp.java\
ConnectionAction.java\
DisplayList.java\
EditField.java\
ErmesModule.java\
ErmesPatcherListener.java\
ErmesSelection.java\
ErmesSketchPad.java\
ErmesSketchWindow.java\
ErmesToolBar.java\
ErmesToolButton.java\
ErrorTableModel.java\
ErrorTablePanel.java\
FinderTableModel.java\
FinderTablePanel.java\
FontCache.java\
IconRenderer.java\
JMaxToolPanel.java\
KeyEventClient.java\
KeyEventsManager.java\
KeyMap.java\
MaxRepaintManager.java\
MessageDisplayer.java\
MoreObjectsButton.java\
ObjectAction.java\
ObjectRenderer.java\
PatcherClipboardManager.java\
PatcherFontManager.java\
PatcherPrintManager.java\
PatcherSaveManager.java\
RuntimeError.java\
RuntimeErrorsTableModel.java\
RuntimeErrorsTablePanel.java\
ScaleDialog.java\
ScaleTransform.java\
Settings.java\
TextRenderer.java\
ToolsPanel.java\
ToolTableModel.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
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


