C_TARGETS=ispw guiobj lists additive eric system
JAVA_TARGETS=table
MIXED_TARGETS=qlist explode


# Late targets must be done at the end, because they depend
# on the mixed target
LATE_C_TARGETS=manoury 

all: all_c all_java
.PHONY: all

all_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k all_c ) ; done
	for i in $(LATE_C_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
.PHONY: all_c

all_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) -k all ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) -k all_java ) ; done
.PHONY: all_java

clean: clean_c clean_java
.PHONY: clean

clean_c:
	for i in $(C_TARGETS) ; do ( cd $$i; $(MAKE) clean ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) clean_c ) ; done
	for i in $(LATE_C_TARGETS) ; do ( cd $$i; $(MAKE) clean ) ; done
.PHONY: clean_c

clean_java:
	for i in $(JAVA_TARGETS) ; do ( cd $$i; $(MAKE) clean ) ; done
	for i in $(MIXED_TARGETS) ; do ( cd $$i; $(MAKE) clean_java ) ; done
.PHONY: clean_java

