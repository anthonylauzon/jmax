SUBDIRS=

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
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
TableExtension.java \
TableGraphicContext.java \
TableInteractionModule.java \
TableMouseDragTracker.java \
TablePanel.java \
TableSelection.java \
Tabler.java \
UndoableValueSet.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
