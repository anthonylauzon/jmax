SUBDIRS=actions menus renderers tools track

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
AbstractValueDataFlavor.java \
Adapter.java \
AmbitusMapper.java \
AmbitusValueDataFlavor.java \
ChannelMapper.java \
CombDrawer.java \
CombListener.java \
DoubleMapper.java \
DurationMapper.java \
FloatAdapter.java \
FloatMapper.java \
FloatValueDataFlavor.java \
FtsSequenceObject.java \
FtsTrackObject.java \
HighlightListener.java \
IntEnumeration.java \
IntegerAdapter.java \
IntegerLabelMapper.java \
IntegerMapper.java \
IntegerValueDataFlavor.java \
LineDrawer.java \
LineListener.java \
Mapper.java \
MappingListener.java \
MessageAdapter.java \
MessageMapper.java \
MessageValueDataFlavor.java \
MidiMouseTracker.java \
MonoDimensionalAdapter.java \
MutexPropertyHandler.java \
PartitionAdapter.java \
PitchLabelMapper.java \
PitchMapper.java \
PositionMapper.java \
ScrollManager.java \
Sequence.java \
SequenceDataFlavor.java \
SequenceDataListener.java \
SequenceDataModel.java \
SequenceExtension.java \
SequenceGraphicContext.java \
SequencePanel.java \
SequenceRuler.java \
SequenceSelecter.java \
SequenceSelection.java \
SequenceSelectionMover.java \
SequenceSelectionResizer.java \
SequenceVerticalResizer.java \
StringMapper.java \
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

JAVACX = @JAVACX@
JAVAROOTX=@top_srcdir@/packages/sequence/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/sequence/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
