SUBDIRS=dialogs editors fts mda script toolkit widgets

EXTRA_DIST = package.html
noinst_PROGRAMS = package
package_SOURCES = \
RecentFileHistory.java\
MaxApplication.java\
MaxToolFinder.java\
MaxVector.java\
MaxVersion.java\
MaxWhenHookTable.java\
MaxWindowManager.java\
ObjectCreatorManager.java\
Platform.java

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/java/lib/jacl/jacl.jar:@top_srcdir@/java/lib/jacl/tcljava.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

