SUBDIRS= actions interactions menus objects

packagedir=$(pkgdatadir)/java/classes/ircam/jmax/editors/patcher
package_JAVA = \
AddPopUp.java\
ConnectionAction.java\
DisplayList.java\
EditField.java\
ErmesModule.java\
ErmesPatcherListener.java\
ErmesSelection.java\
ErmesSketchPad.java\
ErmesSketchWindow.java\
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
ToolBar.java\
ToolButton.java\
ToolTableModel.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@ -sourcepath $(top_srcdir)/java/src
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar
