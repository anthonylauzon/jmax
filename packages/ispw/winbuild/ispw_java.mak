
JAVAC="$(JAVA_HOME)\bin\javac"

JAR="$(JAVA_HOME)\bin\jar"

PKG=ispw



all: dir classes jar



dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"
jar:
	-@cd ..\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam Ispw.class
classes:
	cd ..\java\ircam\jmax
	cd ispw
	$(JAVAC) -classpath ..\..\..\..\..\..\java\jmax.jar;..\..\..\classes;..\..\..\..\src -d ..\..\..\classes *.java
	-@cd ..
	cd ..\..\..\winbuild
	cd ..\java
	$(JAVAC) -classpath ..\..\..\java\jmax.jar;.\classes;. -d .\classes Ispw.java
	cd ..\winbuild

