packagedir=$(pkglibdir)/packages/sequence/java/classes/ircam/jmax/editors/sequence/renderers
package_JAVA = \
AbstractTrackRenderer.java \
AmbitusEventRenderer.java \
AnythingEventRenderer.java \
AnythingTrackBackground.java \
AnythingTrackForeground.java \
AnythingTrackRenderer.java \
FmatEventRenderer.java \
IntegerEventRenderer.java \
IntegerTrackRenderer.java \
MarkerEventRenderer.java \
MessageEventRenderer.java \
MonoTrackBackground.java \
MonoTrackForeground.java \
MonoTrackRenderer.java \
PartitionBackground.java \
ScoreBackground.java \
ScoreForeground.java \
ScoreRenderer.java \
SeqObjectRenderer.java \
SequenceFonts.java \
SequenceImages.java \
TextRenderer.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/sequence/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/sequence/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
