SUBDIRS=actions menus renderers tools track
packagedir=$(pkglibdir)/packages/sequence/java/classes/ircam/jmax/editors/sequence
package_JAVA = \
AbstractValueDataFlavor.java \
Adapter.java \
IntervalMapper.java \
AmbitusValueDataFlavor.java \
AnythingAdapter.java \
ChannelMapper.java \
DoubleMapper.java \
DurationMapper.java \
FloatAdapter.java \
FloatMapper.java \
FloatValueDataFlavor.java \
FtsSequenceObject.java \
FtsTrackObject.java \
FtsTrackEditorObject.java \
HighlightListener.java \
IntEnumeration.java \
IntegerAdapter.java \
IntegerMapper.java \
IntegerValueDataFlavor.java \
LabelsMapper.java \
Mapper.java \
MappingListener.java \
MessageAdapter.java \
MessageMapper.java \
MessageValueDataFlavor.java \
MidiMouseTracker.java \
MonoDimensionalAdapter.java \
MutexPropertyHandler.java \
PartitionAdapter.java \
PitchMapper.java \
PositionMapper.java \
ScrollManager.java \
SequenceClipboard.java \
SequenceDataFlavor.java \
SequenceDataListener.java \
SequenceDataModel.java \
SequenceDefaults.java \
SequenceEditor.java \
SequenceGraphicContext.java \
SequencePanel.java \
SequenceRuler.java \
SequenceSelecter.java \
SequenceSelection.java \
SequenceSelectionMover.java \
SequenceSelectionResizer.java \
SequenceVerticalResizer.java \
SequenceWindow.java \
StringMapper.java \
TempoBar.java \
TimeMapper.java \
TrackContainer.java \
UndoableAdd.java \
UndoableDelete.java \
UndoableEventTransf.java \
UndoableMove.java \
VelocityMapper.java \
VerboseMouseTracker.java \
VerticalScrollable.java \
VerticalZoomable.java \
WholeNumberField.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/sequence/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/sequence/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
