
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=sequence

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam

classes:
	cd ..\java\src\ircam\jmax

	cd editors\sequence
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..

	cd editors\sequence\renderers
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\sequence\tools
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\sequence\actions
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\sequence\menus
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\sequence\track
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\jacl.jar;..\..\..\..\..\..\..\..\..\java\lib\jacl\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd ..\..\..\..\winbuild
