noinst_PROGRAMS = package
package_SOURCES = \
ClosedTrackPopupMenu.java \
EditMenu.java \
FileMenu.java \
MessageTrackPopupMenu.java \
MidiEventPopupMenu.java \
MidiTrackPopupMenu.java \
MonoTrackPopupMenu.java \
RulerPopupMenu.java \
TrackMenu.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/sequence/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/sequence/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
