EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
Bang.java \
BangControlPanel.java \
Comment.java \
Display.java \
FloatBox.java \
Fork.java \
FtsBangObject.java \
FtsCommentObject.java \
FtsDisplayListener.java \
FtsDisplayObject.java \
FtsFloatValueListener.java \
FtsFloatValueObject.java \
FtsForkObject.java \
FtsMessConstObject.java \
FtsScopeObject.java \
FtsSliderObject.java \
FtsVectorDisplayObject.java \
GuiObjExtension.java \
IntBox.java \
MessConst.java \
ObjectControlPanel.java \
Scope.java \
ScopeControlPanel.java \
ScopeRunModePopUp.java \
Slider.java \
SliderControlPanel.java \
Toggle.java \
VecDispControlPanel.java \
VecDispRModePopUp.java \
VectorDisplay.java 

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/packages/guiobj/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/packages/guiobj/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class
