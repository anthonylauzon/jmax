EXTRA_DIST= package.html
noinst_PROGRAMS = package
package_SOURCES = \
AbbreviationCmd.java\
ConsoleOpenCmd.java\
DefaultTclScript.java\
DefineScaleFactorCmd.java\
FtsAbstractionCmd.java\
FtsAbstractionPathCmd.java\
FtsHelpPatchCmd.java\
FtsHelpSummaryCmd.java\
FtsReferenceURLCmd.java\
FtsSyncCmd.java\
FtsTemplateCmd.java\
FtsTemplatePathCmd.java\
FtsUcsCmd.java\
JMaxCmd.java\
MaxDeclareIconCmd.java\
MaxDeclareSystemIconCmd.java\
MaxFtsConnectCmd.java\
MaxGetMaxVersionCmd.java\
MaxOpenCmd.java\
MaxQuitCmd.java\
MaxRunHooksCmd.java\
MaxSetSystemPropertyCmd.java\
MaxSourceCmd.java\
MaxSplashCmd.java\
MaxSysSourceCmd.java\
MaxSystemPropertyCmd.java\
MaxTclDocument.java\
MaxTclExeDocument.java\
MaxTclExeDocumentHandler.java\
MaxTclExeDocumentType.java\
MaxWhenCmd.java\
TclConsolePackage.java\
TclErmesPackage.java\
TclFtsPackage.java\
TclInterpreter.java\
TclMaxPackage.java\
TclMdaPackage.java\
TclScript.java

JAVACX=@JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
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


