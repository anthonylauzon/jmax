SUBDIRS=actions menus

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
AAAReadme.java \
Adapter.java \
ArrowTool.java \
ChannelMapper.java \
DeleteTool.java \
DurationMapper.java \
Explode.java \
ExplodeDataFlavor.java \
ExplodeDataListener.java \
ExplodeDataModel.java \
ExplodeExtension.java \
ExplodeGraphicContext.java \
ExplodePanel.java \
ExplodeSelection.java \
ExplodeSelectionMover.java \
ExplodeSelectionResizer.java \
ExplodeTableModel.java \
ExplodeTablePanel.java \
FtsExplodeObject.java \
Mapper.java \
MappingListener.java \
MoverTool.java \
PartitionAdapter.java \
PartitionEventRenderer.java \
PitchMapper.java \
ResizerTool.java \
ScoreBackground.java \
ScoreForeground.java \
ScoreRenderer.java \
ScrAddingTool.java \
ScrEvent.java \
ScrEventWidget.java \
ScrPanel.java \
SelecterTool.java \
SettingsDialog.java \
TimeMapper.java \
TranspositionListener.java \
UndoableAdd.java \
UndoableDelete.java \
UndoableEventTransformation.java \
UndoableMove.java \
VelocityMapper.java \
VerboseMouseTracker.java \
WholeNumberField.java \
ZoomTool.java 

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
