SUBDIRS=renderers tools

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
Bpf.java \
BpfAdapter.java \
BpfDataListener.java \
BpfDataModel.java \
BpfEditor.java \
BpfExtension.java \
BpfGraphicContext.java \
BpfPanel.java \
BpfPoint.java \
BpfPopupMenu.java \
BpfRuler.java \
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
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
