packagedir=$(pkglibdir)/java/classes/ircam/jmax/editors/patcher/interactions
package_JAVA = \
AddModeInteraction.java\
BgPopUpInteraction.java\
ConnSelectInteraction.java\
ConnToggleSelect.java\
CtrlDoubleClickEditInteraction.java\
DoubleClickEdit.java\
DragConnectInteraction.java\
DragReverseConnect.java\
DragSelectInteraction.java\
DragToggleSelect.java\
EditCtrlInteraction.java\
EditModeInteraction.java\
FollowInteraction.java\
HResizeInteraction.java\
HelpInteraction.java\
Interaction.java\
InteractionEngine.java\
Interactions.java\
MoveConnectInteraction.java\
MoveEditInteraction.java\
MoveInteraction.java\
MoveReverseConnect.java\
PopUpInteraction.java\
RunCtrlInteraction.java\
RunModeInteraction.java\
RunPopUpInteraction.java\
Squeack.java\
VResizeInteraction.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

# We need EXTRA_DIST for automake provided with RedHat 8.0 and RedHat 9
EXTRA_DIST=$(package_JAVA)
