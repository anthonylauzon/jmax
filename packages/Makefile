C_TARGETS= fts1.5 manoury tools eric
JAVA_TARGETS=table qlist

all: all_c all_java
.PHONY: all

all_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
.PHONY: all_c

all_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
.PHONY: all_java

clean: clean_c clean_java
.PHONY: clean

clean_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) -k clean ) ; done
.PHONY: clean_c

clean_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) -k clean ) ; done
.PHONY: clean_java

