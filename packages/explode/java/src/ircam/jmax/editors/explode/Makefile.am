SUBDIRS=actions menus

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
ExplodeGraphicContext.java \
ExplodePanel.java \
ExplodeSelection.java \
ExplodeSelectionMover.java \
ExplodeSelectionResizer.java \
ExplodeTableModel.java \
ExplodeTablePanel.java \
ExplodeWindow.java \
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
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/explode/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/explode/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
