all:
	(cd fts; $(MAKE) all)
	(cd packages; $(MAKE) all)
.PHONY: all

clean:
	(cd fts; $(MAKE) clean)
	(cd packages; $(MAKE) clean)
.PHONY: all
