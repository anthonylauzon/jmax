
rpm: $(SRCFILE)
	mkdir -p /var/tmp/jmax-rpm/src /var/tmp/jmax-rpm/build
	cp $(SRCFILE) /var/tmp/jmax-rpm/src
	rpm -ba --rcfile ./rpmrc jmax.spec
.PHONY: rpm

