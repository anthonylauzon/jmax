noinst_PROGRAMS = package
package_SOURCES = \
CloseAction.java\
DSPAction.java\
DefaultActions.java\
EditorAction.java\
NewAction.java\
OpenAction.java\
OpenHelpSummaryAction.java\
QuitAction.java\
StatisticsAction.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

SUFFIXES= .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

clean-generic:
	rm -f *~


