SUBDIRS=dialogs editors fts toolkit widgets

packagedir=$(pkgdatadir)/java/classes/ircam/jmax
package_JAVA =					\
ConfigPackagePanel.java				\
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
ProjectEditor.java			    	\
RecentFileHistory.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/client/java/ftsclient.jar

