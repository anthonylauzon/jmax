# SUBDIRS= api arch HOWTO images
SUBDIRS= images api

htmldir=$(pkgdatadir)/doc

html_DATA=\
compiling.html \
configuration.html \
debugging.html \
classes.html \
dsp-classes.html \
explode.html \
index.html \
installing.html \
patcher.html \
project-package.html \
ReleaseNotes.html \
running.html

EXTRA_DIST=$(html_DATA)

doc:
	$(MAKE) -C api doc

