# Attention !!! The order is important: since some packages are linked versus others on Mac OS X,
# the linked packages must be compiled first
# Note: the order can be derived from the "require" order

if COND_COMPILE_JACK
JACK_PKG_DIR=jack
else
JACK_PKG_DIR=
endif

if COND_COMPILE_MIDISHARE
MIDISHARE_PKG_DIR=midishare
else
MIDISHARE_PKG_DIR=
endif

#if COND_COMPILE_UNIXDTD
#UNIXDTD_DIR=unixdtd
#else
UNIXDTD_DIR=
#endif

SUBDIRS= \
$(JACK_PKG_DIR) \
$(MIDISHARE_PKG_DIR) \
$(UNIXDTD_DIR) \
@PLATFORM_PACKAGES@ \
asio \
control \
data \
dsdev \
dtd \
functions \
guiobj \
io \
ispw \
ispwmath \
macosx \
midi \
mmio \
numeric \
sequence \
signal \
system \
utils \
winbuild \
winmidi 




