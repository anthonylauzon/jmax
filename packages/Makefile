C_TARGETS=ispw guiobj lists additive manoury eric kimbo
JAVA_TARGETS=table
MIXED_TARGETS=qlist explode

all: all_c all_java
.PHONY: all

all_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k all_c ) ; done
.PHONY: all_c

all_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k all_java ) ; done
.PHONY: all_java

clean: clean_c clean_java
.PHONY: clean

clean_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) -k clean ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k clean_c ) ; done
.PHONY: clean_c

clean_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) -k clean ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k clean_java ) ; done
.PHONY: clean_java

