
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=explode

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam

classes:
	cd ..\java\src\ircam\jmax

	cd editors\explode
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\java\jacl.jar;..\..\..\..\..\..\..\..\java\tcljava.jar;..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..

	cd editors\explode\actions
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\jacl.jar;..\..\..\..\..\..\..\..\..\java\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd editors\explode\menus
	$(JAVAC) -classpath ..\..\..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\..\..\java\jacl.jar;..\..\..\..\..\..\..\..\..\java\tcljava.jar;..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..

	cd ..\..\..\..\winbuild

