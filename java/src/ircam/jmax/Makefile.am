SUBDIRS=dialogs editors fts toolkit widgets

packagedir=$(pkgdatadir)/java/classes/ircam/jmax
package_JAVA =					\
JMaxApplication.java				\
JMaxClassMap.java				\
JMaxIcons.java					\
JMaxObjectCreator.java				\
JMaxPackage.java				\
JMaxPackageLoader.java				\
JMaxPackageLoadingException.java		\
JMaxUtilities.java				\
JMaxVersion.java				\
MaxToolFinder.java				\
MaxVector.java					\
MaxWindowManager.java				\
PackageClassLoader.java				\
Platform.java					\
RecentFileHistory.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= $(top_builddir)/java/src
CLASSPATH_ENV= CLASSPATH=$(CLASSPATH):$(JAVAROOT):$(top_builddir)/client/java/ftsclient.jar:$(top_srcdir)/java/src

