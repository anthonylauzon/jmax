SUBDIRS=actions menus

packagedir=$(pkglibdir)/java/classes/ircam/jmax/toolkit
package_JAVA = \
AbstractLayer.java\
AbstractRenderer.java\
AbstractSelection.java\
AbstractUndoableObject.java\
ClipableData.java\
ComponentContextSwitcher.java\
ContextSwitcher.java\
DirectionChooser.java\
DirectionListener.java\
Displayer.java \
DragListener.java\
DynamicDragListener.java\
Editor.java\
EditorContainer.java\
EditorToolbar.java\
Geometry.java\
GraphicContext.java\
GraphicSelectionListener.java\
InfoPanel.java\
InteractionModule.java\
InteractionSemaphore.java\
JMaxEditor.java\
JMaxMatTable.java\
JMaxTableListener.java\
Layer.java\
LockListener.java\
MouseDragTracker.java\
MouseTracker.java\
NumericPropertyEditor.java\
NumericalWidget.java\
ObjectControlPanel.java\
ObjectRenderer.java\
PopupProvider.java\
PopupToolbarPanel.java\
PositionListener.java\
ProportionalLayout.java\
RenderManager.java\
Selecter.java\
SelectionHandler.java\
SelectionListener.java\
SelectionMover.java\
SelectionOwner.java\
SelectionResizer.java\
StatusBar.java\
StatusBarClient.java\
SwitchListener.java\
Tool.java\
ToolChangeEvent.java\
ToolListener.java\
ToolManager.java\
ToolProvider.java\
ToolbarProvider.java\
TranspositionListener.java\
UndoableData.java\
Widget.java\
WindowContextSwitcher.java\
XORHandler.java\
XORPainter.java\
ZoomListener.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
