ROOTDIR=../../..
JMAX_PROTOCOL_CLASSES=$(ROOTDIR)/lib/protocol/java/classes
JAVAC=javac
JAVA=java

CLASSES=$(shell ls *.java | sed s/.java/.class/)

all: $(CLASSES)

%.class: %.java
	( CLASSPATH=$$CLASSPATH:$(JMAX_PROTOCOL_CLASSES) ; export CLASSPATH ; $(JAVAC) $(JFLAGS) $< )

# Use: make run MAIN=UDPMessageTest ARGS="linotte.ircam.fr 2002 freq vol mod"
run:
	( CLASSPATH=$$CLASSPATH:$(JMAX_PROTOCOL_CLASSES) ; export CLASSPATH ; $(JAVA) $(MAIN) $(ARGS) )
