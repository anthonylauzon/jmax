SUBDIRS= actions interactions menus objects

EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
AddPopUp.java\
ConnectionAction.java\
DisplayList.java\
EditField.java\
ErmesDataEditor.java\
ErmesModule.java\
ErmesPatcherFactory.java\
ErmesPatcherListener.java\
ErmesSelection.java\
ErmesSketchPad.java\
ErmesSketchWindow.java\
ErmesToolBar.java\
ErmesToolButton.java\
FindPanel.java\
FontCache.java\
FpePanel.java\
IconRenderer.java\
KeyEventClient.java\
KeyEventsManager.java\
KeyMap.java\
MaxRepaintManager.java\
MessageDisplayer.java\
MoreObjectsButton.java\
ObjectAction.java\
ObjectRenderer.java\
PatcherClipboardManager.java\
PatcherFontManager.java\
PatcherPrintManager.java\
PatcherSaveManager.java\
ScaleDialog.java\
ScaleTransform.java\
Settings.java\
TextRenderer.java

JAVACX=@JAVACX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES= .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~


