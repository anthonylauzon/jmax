
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=data

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\data\java
	$(JAVAC) -classpath ..\..\..\java\jmax.jar;..\..\..\packages\data\java -d .\classes *.java
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	-@cd classes
	$(JAR) -cf ..\$(PKG).jar ircam *.class

classes:
	cd ..\java\ircam\jmax

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\bpf\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\bpf\renderers\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\bpf\tools\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\table\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\table\renderers\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\table\tools\*.java

	$(JAVAC) -classpath ..\..\..\..\..\java\jmax.jar;..\..\..\..\..\packages\data\java -d ..\..\..\java\classes editors\table\menus\*.java

	cd ..\..\..\..\winbuild

