
JAVAC=$(JAVA_HOME)\javac
JAR=$(JAVA_HOME)\jar
PKG=ispw

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam

classes:
	cd ..\java\src\ircam\jmax

	cd ispw
	$(JAVAC) -classpath ..\..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\..\java\jacl.jar;..\..\..\..\..\..\..\java\tcljava.jar;..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java
	-@cd ..

	cd ..\..\..\..\winbuild

