JAVAC="$(JAVA_HOME)\bin\javac"

JAR="$(JAVA_HOME)\bin\jar"

all: dir jmax jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"
jar:
	-@cd ..\java\classes

	-@if exist "jmax.jar" erase "jmax.jar"

	$(JAR) -cf ..\jmax.jar ircam

	cd ..\..\winbuild

jmax:
	cd ..\java

	cd src\ircam\jmax

	$(JAVAC) -classpath ..\..\..\classes;..\..\..\src -d ..\..\..\classes *.java

	-@cd ..\..\..


	cd src\ircam\jmax\fts

	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java

	-@cd ..\..\..\..



	cd src\ircam\jmax\dialogs

	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java

	-@cd ..\..\..\..



	cd src\ircam\jmax\widgets

	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java

	-@cd ..\..\..\..



	cd src\ircam\jmax\editors\console

	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..



	cd src\ircam\jmax\editors\patcher

	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..



	cd src\ircam\jmax\editors\patcher\actions 

	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..\..



	cd src\ircam\jmax\editors\patcher\menus 

	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..\..



	cd src\ircam\jmax\editors\patcher\interactions 

	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..\..



	cd src\ircam\jmax\editors\patcher\objects 

	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..\..

	cd src\ircam\jmax\toolkit

	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java

	-@cd ..\..\..\..



	cd src\ircam\jmax\toolkit\actions

	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..



	cd src\ircam\jmax\toolkit\menus

	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java

	-@cd ..\..\..\..\..

	cd ..\winbuild

