
JAVAC="$(JAVA_HOME)\bin\javac"
JAR="$(JAVA_HOME)\bin\jar"
PKG=sequence

#Path from winbuild to jmax-cvs/packages/sequence
WINBUILD_TO_PKG=..

# Path from winbuild to jmax-cvs
WINBUILD_TO_TOP=..\..\..

# Path from winbuild to java top src
WINBUILD_TO_JAVA=$(WINBUILD_TO_PKG)\java\ircam\jmax

# Path from java top src to winbuild
JAVA_TO_WINBUILD=..\..\..\winbuild

all: dir classes jar

dir:
	-@if not exist "$(WINBUILD_TO_PKG)\java\classes" mkdir "$(WINBUILD_TO_PKG)\java\classes"

jar:
	-@cd $(WINBUILD_TO_PKG)\java\classes
	-@if exist "$(PKG).jar" erase "$(PKG).jar"
	$(JAR) -cf ..\$(PKG).jar ircam Sequence.class

classes:
	cd $(WINBUILD_TO_JAVA)\editors\sequence
	$(JAVAC) -classpath ..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_JAVA)\editors\sequence\renderers
	$(JAVAC) -classpath ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_JAVA)\editors\sequence\tools
	$(JAVAC) -classpath ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_JAVA)\editors\sequence\actions
	$(JAVAC) -classpath ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_JAVA)\editors\sequence\menus
	$(JAVAC) -classpath ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_JAVA)\editors\sequence\track
	$(JAVAC) -classpath ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_TOP)\java\jmax.jar;..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes;..\..\..\$(JAVA_TO_WINBUILD)\..\java -d ..\..\..\$(JAVA_TO_WINBUILD)\$(WINBUILD_TO_PKG)\java\classes *.java
	-@cd ..\..\..\$(JAVA_TO_WINBUILD)

	cd $(WINBUILD_TO_PKG)\java
	$(JAVAC) -classpath ..\winbuild\$(WINBUILD_TO_TOP)\java\jmax.jar;.\classes;. -d .\classes Sequence.java
	-@cd ..\winbuild

