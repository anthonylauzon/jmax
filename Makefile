all:
	(cd fts; $(MAKE) all)
	(cd packages; $(MAKE) all)
	(cd java ; $(MAKE) all)
.PHONY: all

clean:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
.PHONY: all
