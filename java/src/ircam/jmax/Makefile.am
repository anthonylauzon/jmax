SUBDIRS=dialogs editors fts toolkit widgets

noinst_PROGRAMS = package
package_SOURCES = \
DefaultIcons.java\
DefaultPackages.java\
JMaxPackage.java\
JMaxVersion.java\
MaxApplication.java\
MaxToolFinder.java\
MaxVector.java\
MaxWindowManager.java\
ObjectCreatorManager.java\
PackageClassLoader.java\
Platform.java\
RecentFileHistory.java

JAVACX = @JAVACX@
JAVACFLAGSX=@JAVACFLAGSX@
JAVAROOTX=@top_srcdir@/java/src
CLASSPATH_ENVX=CLASSPATH=@top_srcdir@/java/src:@top_srcdir@/client/java/ftsclient.jar

SUFFIXES = .java .class

.java.class:
	$(CLASSPATH_ENVX) $(JAVACX) -d $(JAVAROOTX) $(JAVACFLAGSX) $?

package: $(package_OBJECTS) $(package_DEPENDENCIES)

clean-compile:
	rm -f *.class

