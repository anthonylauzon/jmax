SUBDIRS=dialogs editors fts toolkit widgets

packagedir=$(pkgdatadir)
package_JAVA =					\
DefaultIcons.java				\
JMaxObjectCreator.java				\
JMaxPackage.java				\
JMaxPackageClassLoader.java			\
JMaxPackageLoader.java				\
JMaxPackageLoadingException.java		\
JMaxVersion.java				\
MaxApplication.java				\
MaxToolFinder.java				\
MaxVector.java					\
MaxWindowManager.java				\
ObjectCreatorManager.java			\
Platform.java					\
RecentFileHistory.java

JAVAC= @JAVACX@
JAVACFLAGS= @JAVACFLAGSX@
JAVAROOT= @top_srcdir@/java/src
CLASSPATH_ENV= CLASSPATH=$(JAVAROOT):@top_srcdir@/client/java/ftsclient.jar

