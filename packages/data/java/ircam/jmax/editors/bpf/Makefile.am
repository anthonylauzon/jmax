SUBDIRS=renderers tools
packagedir=$(pkglibdir)/packages/data/java/classes/ircam/jmax/editors/bpf
package_JAVA = \
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

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/data/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/data/java

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
