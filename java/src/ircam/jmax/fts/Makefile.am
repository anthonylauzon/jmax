noinst_PROGRAMS = package
package_SOURCES = \
FtsAtomList.java\
FtsAtomListListener.java\
FtsClipboard.java\
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
FtsHelpPatchTable.java\
FtsInletObject.java\
FtsInletsListener.java\
FtsIntValueListener.java\
FtsIntValueObject.java\
FtsMessageListener.java\
FtsModule.java\
FtsObjectErrorListener.java\
FtsObjectSet.java\
FtsObjectWithEditor.java\
FtsOutletObject.java\
FtsOutletsListener.java\
FtsParse.java\
FtsPatcherListener.java\
FtsPatcherObject.java\
FtsTemplateObject.java\
FtsQuittedException.java\
FtsRuntimeErrors.java\
FtsSelection.java\
FtsUndoableObject.java\
FtsUpdateGroupListener.java\
FtsActionListener.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~
