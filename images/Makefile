# Generated automatically from Makefile.in by configure.
# Makefile.in generated automatically by automake 1.4-p5 from Makefile.am

# Copyright (C) 1994, 1995-8, 1999, 2001 Free Software Foundation, Inc.
# This Makefile.in is free software; the Free Software Foundation
# gives unlimited permission to copy and/or distribute it,
# with or without modifications, as long as this notice is preserved.

# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY, to the extent permitted by law; without
# even the implied warranty of MERCHANTABILITY or FITNESS FOR A
# PARTICULAR PURPOSE.


SHELL = /bin/sh

srcdir = .
top_srcdir = ..
prefix = /u/worksta/borghesi/projects/jmax-install
exec_prefix = ${prefix}

bindir = ${exec_prefix}/bin
sbindir = ${exec_prefix}/sbin
libexecdir = ${exec_prefix}/libexec
datadir = ${prefix}/share
sysconfdir = ${prefix}/etc
sharedstatedir = ${prefix}/com
localstatedir = ${prefix}/var
libdir = ${exec_prefix}/lib
infodir = ${prefix}/info
mandir = ${prefix}/man
includedir = ${prefix}/include
oldincludedir = /usr/include

DESTDIR =

pkgdatadir = $(datadir)/jmax
pkglibdir = $(libdir)/jmax
pkgincludedir = $(includedir)/jmax

top_builddir = ..

ACLOCAL = aclocal
AUTOCONF = autoconf
AUTOMAKE = automake
AUTOHEADER = autoheader

INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL} $(AM_INSTALL_PROGRAM_FLAGS)
INSTALL_DATA = ${INSTALL} -m 644
INSTALL_SCRIPT = ${INSTALL_PROGRAM}
transform = s,x,x,

NORMAL_INSTALL = :
PRE_INSTALL = :
POST_INSTALL = :
NORMAL_UNINSTALL = :
PRE_UNINSTALL = :
POST_UNINSTALL = :
host_alias = i686-pc-linux-gnu
host_triplet = i686-pc-linux-gnu
ALSA_CFLAGS = 
ALSA_LIBS =  -lasound -lm -ldl
AS = @AS@
AUDIOFILE_CFLAGS = 
AUDIOFILE_CONFIG = /usr/bin/audiofile-config
AUDIOFILE_LIBS = -L/usr/lib -laudiofile -lm
CC = gcc
CONFIG_FILE = config_linux_alsa.jprj
DLLTOOL = @DLLTOOL@
DOT_LIBS = 
ECHO = echo
EXEEXT = 
FTSDIR = ${exec_prefix}/bin
FTS_SYS_LIBS = -ldl -lpthread 
JAVA = java
JAVAC = javac
JAVACFLAGS = @JAVACFLAGS@
JAVACFLAGSX =  -g
JAVACX = javac
JAVAFLAGS = @JAVAFLAGS@
JAVAPREFIX = @JAVAPREFIX@
LEX = flex
LIBTOOL = $(SHELL) $(top_builddir)/libtool
LN_S = ln -s
MAKEINFO = makeinfo
OBJDUMP = @OBJDUMP@
OBJEXT = o
PACKAGE = jmax
PLATFORM = linux
PLATFORM_PACKAGES = alsa unixdtd aflib oss
RANLIB = ranlib
ROOTDIR = ${prefix}/share/jmax
STRIP = strip
VERSION = 3.1.0_latest_cvs
YACC = bison -y
uudecode = yes

pkgimgdir = $(pkgdatadir)/images
pkgimg_DATA = \
Splash.gif \
arrow_down.gif \
arrow_left.gif \
arrow_right.gif \
arrow_up.gif \
at_work.gif \
cursor_bang_mini.gif \
cursor_in.gif \
cursor_out.gif \
cursor_patcher.gif \
cursor_standard.gif \
dsp_off.gif \
dsp_on.gif \
dsp_undef.gif \
forum.gif \
hot.gif \
info.gif \
ircam.gif \
jmax_app.gif \
jmax.icns \
jmax_icon.gif \
jmax_logo_small.gif \
jmax_logo_small_transp.gif \
jmax_logo_tiny.gif \
linux.gif \
little_down_arrow.gif \
little_minus.gif \
little_plus.gif \
little_up_arrow.gif \
mini_icon_jmax.gif \
mini_icon_pat.gif \
more_objects.gif \
new.gif \
project_root.gif \
question.gif \
stop.gif \
to_save.gif \
tool_edit_mode.gif \
tool_err.gif \
tool_ext.gif \
tool_in.gif \
tool_lock_mode.gif \
tool_out.gif \
tool_patcher.gif \
tool_text.gif \
tool_up.gif \
tux.gif \
warning.gif 


EXTRA_DIST = $(pkgimg_DATA)
mkinstalldirs = $(SHELL) $(top_srcdir)/mkinstalldirs
CONFIG_HEADER = ../include/ftsconfig-ac.h
CONFIG_CLEAN_FILES = 
DATA =  $(pkgimg_DATA)

DIST_COMMON =  Makefile.am Makefile.in


DISTFILES = $(DIST_COMMON) $(SOURCES) $(HEADERS) $(TEXINFOS) $(EXTRA_DIST)

TAR = gtar
GZIP_ENV = --best
all: all-redirect
.SUFFIXES:
$(srcdir)/Makefile.in: Makefile.am $(top_srcdir)/configure.in $(ACLOCAL_M4) 
	cd $(top_srcdir) && $(AUTOMAKE) --foreign images/Makefile

Makefile: $(srcdir)/Makefile.in  $(top_builddir)/config.status $(BUILT_SOURCES)
	cd $(top_builddir) \
	  && CONFIG_FILES=$(subdir)/$@ CONFIG_HEADERS= $(SHELL) ./config.status


install-pkgimgDATA: $(pkgimg_DATA)
	@$(NORMAL_INSTALL)
	$(mkinstalldirs) $(DESTDIR)$(pkgimgdir)
	@list='$(pkgimg_DATA)'; for p in $$list; do \
	  if test -f $(srcdir)/$$p; then \
	    echo " $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgimgdir)/$$p"; \
	    $(INSTALL_DATA) $(srcdir)/$$p $(DESTDIR)$(pkgimgdir)/$$p; \
	  else if test -f $$p; then \
	    echo " $(INSTALL_DATA) $$p $(DESTDIR)$(pkgimgdir)/$$p"; \
	    $(INSTALL_DATA) $$p $(DESTDIR)$(pkgimgdir)/$$p; \
	  fi; fi; \
	done

uninstall-pkgimgDATA:
	@$(NORMAL_UNINSTALL)
	list='$(pkgimg_DATA)'; for p in $$list; do \
	  rm -f $(DESTDIR)$(pkgimgdir)/$$p; \
	done
tags: TAGS
TAGS:


distdir = $(top_builddir)/$(PACKAGE)-$(VERSION)/$(subdir)

subdir = images

distdir: $(DISTFILES)
	here=`cd $(top_builddir) && pwd`; \
	top_distdir=`cd $(top_distdir) && pwd`; \
	distdir=`cd $(distdir) && pwd`; \
	cd $(top_srcdir) \
	  && $(AUTOMAKE) --include-deps --build-dir=$$here --srcdir-name=$(top_srcdir) --output-dir=$$top_distdir --foreign images/Makefile
	@for file in $(DISTFILES); do \
	  d=$(srcdir); \
	  if test -d $$d/$$file; then \
	    cp -pr $$d/$$file $(distdir)/$$file; \
	  else \
	    test -f $(distdir)/$$file \
	    || ln $$d/$$file $(distdir)/$$file 2> /dev/null \
	    || cp -p $$d/$$file $(distdir)/$$file || :; \
	  fi; \
	done
info-am:
info: info-am
dvi-am:
dvi: dvi-am
check-am: all-am
check: check-am
installcheck-am:
installcheck: installcheck-am
install-exec-am:
install-exec: install-exec-am

install-data-am: install-pkgimgDATA
install-data: install-data-am

install-am: all-am
	@$(MAKE) $(AM_MAKEFLAGS) install-exec-am install-data-am
install: install-am
uninstall-am: uninstall-pkgimgDATA
uninstall: uninstall-am
all-am: Makefile $(DATA)
all-redirect: all-am
install-strip:
	$(MAKE) $(AM_MAKEFLAGS) AM_INSTALL_PROGRAM_FLAGS=-s install
installdirs:
	$(mkinstalldirs)  $(DESTDIR)$(pkgimgdir)


mostlyclean-generic:

clean-generic:

distclean-generic:
	-rm -f Makefile $(CONFIG_CLEAN_FILES)
	-rm -f config.cache config.log stamp-h stamp-h[0-9]*

maintainer-clean-generic:
mostlyclean-am:  mostlyclean-generic

mostlyclean: mostlyclean-am

clean-am:  clean-generic mostlyclean-am

clean: clean-am

distclean-am:  distclean-generic clean-am
	-rm -f libtool

distclean: distclean-am

maintainer-clean-am:  maintainer-clean-generic distclean-am
	@echo "This command is intended for maintainers to use;"
	@echo "it deletes files that may require special tools to rebuild."

maintainer-clean: maintainer-clean-am

.PHONY: uninstall-pkgimgDATA install-pkgimgDATA tags distdir info-am \
info dvi-am dvi check check-am installcheck-am installcheck \
install-exec-am install-exec install-data-am install-data install-am \
install uninstall-am uninstall all-redirect all-am all installdirs \
mostlyclean-generic distclean-generic clean-generic \
maintainer-clean-generic clean mostlyclean distclean maintainer-clean


# Tell versions [3.59,3.63) of GNU make to not export all variables.
# Otherwise a system limit (for SysV at least) may be exceeded.
.NOEXPORT:
