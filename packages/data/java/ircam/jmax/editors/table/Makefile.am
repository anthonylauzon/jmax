SUBDIRS= menus renderers tools

packagedir=$(pkgdatadir)/packages/data/java/classes/ircam/jmax/editors/table
package_JAVA = \
Actions.java \
FreeHandDrawer.java \
FtsFvecObject.java \
FtsIvecObject.java \
FtsTableObject.java \
LineDrawer.java \
LineListener.java \
StripeSelecter.java \
TableAdapter.java \
TableDataListener.java \
TableDataModel.java \
TableDisplay.java \
TableDynamicDragListener.java \
TableGraphicContext.java \
TableInteractionModule.java \
TableMouseDragTracker.java \
TablePanel.java \
TableSelection.java \
TableToolManager.java \
Tabler.java \
UndoableValueSet.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@
JAVAROOT=$(top_builddir)/packages/data/java
CLASSPATH_ENV=CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/packages/data/java
