packagedir=$(pkgdatadir)/packages/ispw/java/classes
package_JAVA = \
FtsAtomList.java \
FtsAtomListListener.java \
FtsMessageObject.java \
Message.java 

JAVAC = @JAVACX@
JAVACFLAGS=@JAVACFLAGSX@ -sourcepath $(top_srcdir)/packages/ispw/java
JAVAROOT=$(top_builddir)/packages/ispw/java
CLASSPATH_ENV=CLASSPATH=$(JAVAROOT):$(top_builddir)/java/src:$(top_builddir)/client/java/ftsclient.jar

