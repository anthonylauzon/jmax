
CONFIGURATION_FILES = \
	default_config_linux_alsa.jcfg \
	default_config_linux_jack.jcfg \
	default_config_linux_oss.jcfg \
	default_config_macosx.jcfg 

PROJECT_FILE = default_project.jprj 


EXTRA_DIST= $(CONFIGURATION_FILES) $(PROJECT_FILE)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(top_srcdir)/config/$(PROJECT_FILE) $(DESTDIR)$(pkgdatadir)/default-project.jprj
	$(INSTALL_DATA) $(top_srcdir)/config/@DEFAULT_CONFIG_FILE@ $(DESTDIR)$(pkgdatadir)/default-config.jcfg
