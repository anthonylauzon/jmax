
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=qlist

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam

classes:
	cd ..\java\src\ircam\jmax

	cd editors\qlist
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..

	cd editors\qlist\actions
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\qlist\menus
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd ..\..\..\..\winbuild

