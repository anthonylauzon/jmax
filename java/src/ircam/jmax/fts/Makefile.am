packagedir=$(pkglibdir)/java/classes/ircam/jmax/fts
package_JAVA = \
FtsAudioLabel.java \
FtsAudioConfig.java\
FtsClipboard.java\
FtsConfig.java\
FtsConnection.java\
FtsConnectionListener.java\
FtsConsole.java\
FtsDspControl.java\
FtsEditListener.java\
FtsErrorStreamer.java\
FtsException.java\
FtsFinderObject.java\
FtsErrorFinderObject.java\
FtsGraphicListener.java\
FtsGraphicObject.java\
FtsHelpPatchTable.java\
FtsInletsListener.java\
FtsIntValueListener.java\
FtsIntValueObject.java\
FtsMessageListener.java\
FtsMidiConfig.java\
FtsObjectErrorListener.java\
FtsObjectSet.java\
FtsObjectWithEditor.java\
FtsOutletsListener.java\
FtsPackage.java\
FtsPatcherListener.java\
FtsPatcherObject.java\
FtsProject.java\
FtsTemplateObject.java\
FtsQuittedException.java\
FtsRuntimeErrors.java\
FtsSelection.java\
FtsUndoableObject.java\
FtsUpdateGroup.java\
FtsUpdateGroupListener.java\
FtsActionListener.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
