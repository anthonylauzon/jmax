packagedir=$(pkgdatadir)/packages/jack/c
package_LTLIBRARIES=libjmax-jack.la
libjmax_jack_la_SOURCES= \
jack.c \
jackaudiomanager.c \
jackaudioport.c 

CFLAGS = @CFLAGS@ @JACK_CFLAGS@
libjmax_jack_la_LDFLAGS= @LDFLAGS@ @JACK_LDFLAGS@
INCLUDES=-I$(top_srcdir)/include

noinst_HEADERS = jackaudio.h
install-data-hook:
	-rm $(DESTDIR)$(packagedir)/libjack.so
	ln $(DESTDIR)$(packagedir)/libjmax-jack.so $(DESTDIR)$(packagedir)/libjack.so

# Must be uninstall-hook but there is a bug in automake 1.4-pr5 which doesn't call uninstall-hook 
# see http://mail.gnu.org/pipermail/automake-patches/2002-January/000546.html
uninstall-local:
	rm $(DESTDIR)$(packagedir)/libjack.so
