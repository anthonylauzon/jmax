noinst_PROGRAMS = package
package_SOURCES = \
AmbitusEventRenderer.java \
AnythingEventRenderer.java \
AnythingTrackBackground.java \
AnythingTrackForeground.java \
AnythingTrackRenderer.java \
IntegerEventRenderer.java \
IntegerTrackRenderer.java \
MessageEventRenderer.java \
MonoTrackBackground.java \
MonoTrackForeground.java \
MonoTrackRenderer.java \
PartitionBackground.java \
ScoreBackground.java \
ScoreForeground.java \
ScoreRenderer.java \
SeqObjectRenderer.java \
SequenceImages.java \
TextRenderer.java 

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
