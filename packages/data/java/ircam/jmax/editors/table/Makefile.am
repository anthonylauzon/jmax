SUBDIRS= menus renderers tools

noinst_PROGRAMS = package
package_SOURCES = \
Actions.java \
ChangeRangeDialog.java \
CoordinateWriter.java \
FreeHandDrawer.java \
FtsTableObject.java \
LineDrawer.java \
LineListener.java \
ScalePanel.java \
StripeSelecter.java \
TableAdapter.java \
TableDataListener.java \
TableDataModel.java \
TableDisplay.java \
TableDynamicDragListener.java \
TableGraphicContext.java \
TableInteractionModule.java \
TableMouseDragTracker.java \
TablePanel.java \
TableSelection.java \
Tabler.java \
UndoableValueSet.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/data/java
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/data/java

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class



