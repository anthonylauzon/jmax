SUBDIRS=

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
AbstractEventValue.java \
AbstractValueInfo.java \
AmbitusValue.java \
ArrayEnumeration.java \
ChangeRangeDialog.java \
ChangeTrackNameDialog.java \
Drawable.java \
Editable.java \
Event.java \
EventValue.java \
FloatTrackEditor.java \
FloatTrackEditorFactory.java \
FloatValue.java \
IntegerTrackEditor.java \
IntegerTrackEditorFactory.java \
IntegerValue.java \
ListContainer.java \
LockListener.java \
MessageTrackEditor.java \
MessageTrackEditorFactory.java \
MessageValue.java \
MidiTrackEditor.java \
MidiTrackEditorFactory.java \
MidiValue.java \
MonoTrackEditor.java \
MonoTrackEditorFactory.java \
NullEditor.java \
SequenceTableDialog.java \
SequenceTablePanel.java \
SequenceTextArea.java \
SequenceTextAreaListener.java \
TextDialog.java \
ToggleBar.java \
Track.java \
TrackBase.java \
TrackDataListener.java \
TrackDataModel.java \
TrackEditor.java \
TrackEditorFactory.java \
TrackEditorFactoryTable.java \
TrackEvent.java \
TrackListListener.java \
TrackListener.java \
TrackTableModel.java \
UtilTrackEvent.java \
ValueEditor.java \
ValueInfo.java \
ValueInfoTable.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/sequence/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/sequence/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
