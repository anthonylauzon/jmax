# Attention !!! The order is important: since some packages are linked versus others on Mac OS X,
# the linked packages must be compiled first
# Note: the order can be derived from the "require" order

if COND_COMPILE_JACK
JACK_PKG_DIR=jack
else
JACK_PKG_DIR=
endif

SUBDIRS=utils control data guiobj io ispw ispwmath midi numeric sequence signal system dtd @PLATFORM_PACKAGES@ $(JACK_PKG_DIR)


