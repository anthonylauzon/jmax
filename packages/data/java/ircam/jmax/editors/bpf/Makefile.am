SUBDIRS=renderers tools

noinst_PROGRAMS = package
package_SOURCES = \
BpfWindow.java \
BpfAdapter.java \
BpfDataListener.java \
BpfDataModel.java \
BpfEditor.java \
BpfGraphicContext.java \
BpfPanel.java \
BpfPoint.java \
BpfPopupMenu.java \
BpfSelection.java \
BpfSelectionMover.java \
BpfTableDialog.java \
BpfTableModel.java \
BpfTablePanel.java \
BpfToolManager.java \
Drawable.java \
FtsBpfObject.java \
ScrollManager.java \
UtilBpfPoint.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/data/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar:@top_srcdir@/packages/data/java/src

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class


