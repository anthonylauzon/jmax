# SUBDIRS= api arch HOWTO images
if COND_DOXYGEN
SUBDIRS= images api
else
SUBDIRS= images
endif

htmldir=$(pkgdatadir)/doc

# html_DATA=\
# compiling.html \
# configuration.html \
# debugging.html \
# classes.html \
# dsp-classes.html \
# explode.html \
# index.html \
# installing.html \
# patcher.html \
# project-package.html \
# ReleaseNotes.html \
# running.html
html_DATA = \
compiling.html \
debugging.html \
classes.html \
explode.html \
index.html \
installing.html \
patcher.html 



EXTRA_DIST=$(html_DATA)

doc:
	$(MAKE) -C api doc

