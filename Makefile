all:
	(cd fts; $(MAKE) all)
	(cd java ; $(MAKE) all)
	(cd packages; $(MAKE) all)
.PHONY: all


clean:
	(cd fts; $(MAKE) clean)
	(cd java ; $(MAKE) clean)
	(cd packages; $(MAKE) clean)
.PHONY: clean

# To make a release, the following must be done logged as jmax
# 1: cvs update -d -P
# 2: copy by hand what is not under CVS (images,...)
# 3: for all architectures, make clean all
# 4: make tar
#      This will create tar file for installation.
# 5: test
# Once test is OK
# 6: make version
#      This will copy the tar file to archive, do a cvs tag,
#      update the version number and commit it.

tar:
	scripts/make-distrib . /tmp/max $(ARCH) t
	(cd /tmp ; tar cvf - max) | gzip > jmax.$(ARCH).`cat VERSION`.tar.gz
	/bin/rm -rf /tmp/max
.PHONY: tar


version:
	echo cp jmax.$(ARCH).`cat VERSION`.tar.gz /u/worksta/jmax/archive
	echo rsh maelzel cd projects/max \; cvs tag -F `awk -F . 'NF==3 { m=$$1; n=$$2; p=$$2; } END { printf("V%d_%d_%d\n",m,n,p);}' VERSION`
	scripts/make-new-version VERSION fts/src/sys/version.h java/src/ircam/jmax/MaxVersion.java
	echo rsh maelzel cd projects/max \; cvs commit -l -m "Automatic commit for new version." VERSION fts/src/sys/version.h java/src/ircam/jmax/MaxVersion.java
.PHONY: version


