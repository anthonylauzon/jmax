SUBDIRS= actions interactions menus objects

packagedir=$(pkglibdir)/java/classes/ircam/jmax/editors/patcher
package_JAVA = \
AddPopUp.java\
ConnectionAction.java\
DisplayList.java\
EditField.java\
ErmesSelection.java\
ErmesSketchPad.java\
ErmesSketchWindow.java\
ErrorTableModel.java\
ErrorTablePanel.java\
Finder.java \
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
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
