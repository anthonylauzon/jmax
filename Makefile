all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd packages; $(MAKE) all)
.PHONY: all

clean:
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd packages; $(MAKE) clean)
.PHONY: all
