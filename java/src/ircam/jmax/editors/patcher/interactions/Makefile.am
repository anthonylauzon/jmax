EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
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


