
JAVAC="$(JAVA_HOME)\bin\javac"

JAR="$(JAVA_HOME)\bin\jar"

PKG=ftsclient



all: dir classes jar



dir:
	-@if not exist "..\..\java\classes" mkdir "..\..\java\classes"



jar:
	-@cd ..\..\java\classes

	-@if exist "$(PKG).jar" erase "$(PKG).jar"

	$(JAR) -cf ..\$(PKG).jar ircam



classes:
	cd ..\java\ircam\fts\client


	$(JAVAC) -d ..\..\..\..\..\java\classes *.java

	-@cd ..\..


	cd ..\..\winbuild



