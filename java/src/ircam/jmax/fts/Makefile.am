noinst_PROGRAMS = package
package_SOURCES = \
Fts.java\
FtsAtom.java\
FtsAtomList.java\
FtsAtomListListener.java\
FtsBmaxRemoteDocumentHandler.java\
FtsClientProtocol.java\
FtsClipboard.java\
FtsConnection.java\
FtsConnectionListener.java\
FtsDatagramClientStream.java\
FtsDatagramStream.java\
FtsDotPatRemoteDocumentHandler.java\
FtsDspControl.java\
FtsEditListener.java\
FtsErrorStreamer.java\
FtsException.java\
FtsFinderObject.java\
FtsErrorFinderObject.java\
FtsHelpPatchTable.java\
FtsInletObject.java\
FtsInletsListener.java\
FtsIntValueListener.java\
FtsIntValueObject.java\
FtsMessageListener.java\
FtsModule.java\
FtsObject.java\
FtsObjectErrorListener.java\
FtsObjectSet.java\
FtsObjectWithEditor.java\
FtsOutletObject.java\
FtsOutletsListener.java\
FtsParse.java\
FtsPatcherDocument.java\
FtsPatcherDocumentType.java\
FtsPatcherListener.java\
FtsPatcherObject.java\
FtsTemplateObject.java\
FtsQuittedException.java\
FtsReferenceURLTable.java\
FtsRuntimeErrors.java\
FtsSelection.java\
FtsServer.java\
FtsSocketClientStream.java\
FtsSocketServerStream.java\
FtsSocketStream.java\
FtsStream.java\
FtsSubProcessStream.java\
FtsSymbol.java\
FtsUndoableObject.java\
FtsUpdateGroupListener.java\
FtsActionListener.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~
