
CONFIGURATION_FILES = \
	default_config_linux_alsa.jcfg


PROJECT_FILES = \
	default_project.jprj \
	default_project_linux_alsa.jprj \
	default_project_linux_jack.jprj \
	default_project_linux_oss.jprj \
	default_project_macosx.jprj \
	default_project_win32.jprj


EXTRA_DIST= $(CONFIGURATION_FILES) $(PROJECT_FILES)

install-data-local:
	$(mkinstalldirs) $(DESTDIR)$(pkgdatadir)
	$(INSTALL_DATA) $(top_srcdir)/config/@DEFAULT_PROJECT_FILE@ $(DESTDIR)$(pkgdatadir)/default-project.jprj
#	$(INSTALL_DATA) $(top_srcdir)/config/@DEFAULT_CONFIG_FILE@ $(DESTDIR)$(pkgdatadir)/default-config.jprj
