
CONFIGURATION_FILES = \
	default_config_linux_alsa.jcfg \
	default_config_linux_jack.jcfg \
	default_config_linux_oss.jcfg \
	default_config_macosx.jcfg 

PROJECT_FILE = default_project.jprj 


EXTRA_DIST= $(CONFIGURATION_FILES) $(PROJECT_FILE)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkglibdir)
	$(INSTALL_DATA) $(top_srcdir)/config/$(PROJECT_FILE) $(DESTDIR)$(pkglibdir)/default_project.jprj
	$(INSTALL_DATA) $(top_srcdir)/config/@DEFAULT_CONFIG_FILE@ $(DESTDIR)$(pkglibdir)/default_config.jcfg

uninstall-local:
	rm -f $(DESTDIR)$(pkglibdir)/default_project.jprj
	rm -f $(DESTDIR)$(pkglibdir)/default_config.jcfg
