
JAVAC=$(JAVA_HOME)\javac
JAR=$(JAVA_HOME)\jar

all: dir silk jmax jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\java\classes
	-@if exist "jmax.jar" erase "jmax.jar"
	$(JAR) -cf ..\jmax.jar ircam silk
	cd ..\..\winbuild

silk:
	cd ..\java\src\silk
	$(JAVAC) -classpath ..\..\src -d ..\..\classes *.java
	-@cd ..\..\..\winbuild

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

	cd src\ircam\jmax\editors\console\actions 
	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..\..

	cd src\ircam\jmax\editors\console\menus 
	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..\..

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

	cd src\ircam\jmax\mda
	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java
	-@cd ..\..\..\..

	cd src\ircam\jmax\toolkit
	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java
	-@cd ..\..\..\..

	cd src\ircam\jmax\toolkit\actions
	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..

	cd src\ircam\jmax\toolkit\menus
	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..

	cd src\ircam\jmax\script
	$(JAVAC) -classpath ..\..\..\..\classes;..\..\..\..\src -d ..\..\..\..\classes *.java
	-@cd ..\..\..\..

	cd src\ircam\jmax\script\pkg
	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..

	cd src\ircam\jmax\script\tcl
	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src;..\..\..\..\..\jacl.jar;..\..\..\..\..\tcljava.jar -d ..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..

	cd src\ircam\jmax\script\scm
	$(JAVAC) -classpath ..\..\..\..\..\classes;..\..\..\..\..\src -d ..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..

	cd src\ircam\jmax\script\scm\silk
	$(JAVAC) -classpath ..\..\..\..\..\..\classes;..\..\..\..\..\..\src -d ..\..\..\..\..\..\classes *.java
	-@cd ..\..\..\..\..\..

	cd ..\winbuild
