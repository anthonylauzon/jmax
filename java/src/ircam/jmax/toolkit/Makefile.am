SUBDIRS=actions menus

EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
AbstractLayer.java\
AbstractRenderer.java\
AbstractSelection.java\
AbstractUndoableObject.java\
ClipableData.java\
ComponentContextSwitcher.java\
ContextSwitcher.java\
Cursors.java\
DirectionChooser.java\
DirectionListener.java\
DragListener.java\
DynamicDragListener.java\
Editor.java\
EditorContainer.java\
EditorToolbar.java\
Geometry.java\
GraphicContext.java\
GraphicSelectionListener.java\
IconCache.java\
Icons.java\
InfoPanel.java\
InteractionModule.java\
InteractionSemaphore.java\
Layer.java\
LockListener.java\
MaxToolsJMenu.java\
MaxToolsMenu.java\
MaxWindowJMenu.java\
MaxWindowMenu.java\
MouseDragTracker.java\
MouseTracker.java\
NumericPropertyEditor.java\
NumericalWidget.java\
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
SystemIcons.java\
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


