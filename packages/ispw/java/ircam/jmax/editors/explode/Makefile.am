SUBDIRS=actions menus

packagedir=$(pkglibdir)/packages/ispw/java/classes/ircam/jmax/editors/explode
package_JAVA = \
AAAReadme.java \
Adapter.java \
ArrowTool.java \
ChannelMapper.java \
DeleteTool.java \
DurationMapper.java \
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

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/ispw/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
