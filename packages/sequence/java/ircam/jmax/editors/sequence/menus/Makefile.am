packagedir=$(pkglibdir)/packages/sequence/java/classes/ircam/jmax/editors/sequence/menus
package_JAVA = \
ClosedTrackPopupMenu.java \
EditMenu.java \
FileMenu.java \
MidiTrackPopupMenu.java \
MonoTrackPopupMenu.java \
RulerPopupMenu.java \
TempoBarPopupMenu.java \
TrackBasePopupMenu.java \
TrackMenu.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/sequence/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/sequence/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
