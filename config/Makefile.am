
EXTRA_DIST=config.jprj config_linux_alsa.jprj config_linux_oss.jprj config_macosx.jprj config_win32.jprj

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(top_srcdir)/config/@CONFIG_FILE@ $(DESTDIR)$(pkgdatadir)/config.jprj
