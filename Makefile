
all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd packages; $(MAKE) all)

all_c:
	(cd fts; $(MAKE) all)
	(cd packages; $(MAKE) all_c)

all_java:
	(cd java ; $(MAKE) all)
	(cd packages; $(MAKE) all_java)

clean:
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd packages; $(MAKE) clean)

clean_c:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean_c)

clean_java:
	(cd java ; $(MAKE) clean)
	(cd packages; $(MAKE) clean_java)

.PHONY: all
.PHONY: all_c
.PHONY: all_java

.PHONY: clean
.PHONY: clean_c
.PHONY: clean_java

