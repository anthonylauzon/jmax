# SUBDIRS= api arch HOWTO images
SUBDIRS= images api

htmldir=$(pkgdatadir)/doc

html_DATA=\
compiling.html \
configuration.html \
debugging.html \
dev-control-objects.html \
dev-dsp-objects.html \
explode.html \
index.html \
installing.html \
patcher.html \
project-package.html \
ReleaseNotes.html \
running.html \
server-kernel-api.html 



EXTRA_DIST=$(html_DATA)

doc:
	$(MAKE) -C api doc

