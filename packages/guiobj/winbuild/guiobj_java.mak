
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=guiobj

all: dir classes jar

dir:
	-@if not exist "..\java\classes" mkdir "..\java\classes"

jar:
	-@cd ..\guiobj\java
	$(JAVAC) -classpath ..\..\..\java\jmax.jar;..\..\..\packages\guiobj\java -d .\classes *.java
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	-@cd classes
	$(JAR) -cf ..\$(PKG).jar ircam  *.class
	-@cd ..
	$(JAR) -uf $(PKG).jar icons

classes:
	cd ..\java\ircam\jmax

	cd guiobj
	$(JAVAC) -classpath ..\..\..\..\..\..\java\jmax.jar;..\..\..\..\..\..\packages\guiobj\java -d ..\..\..\..\java\classes *.java
	-@cd ..

	cd ..\..\..\..\winbuild

