
## types and editors definitions

# patcher data type and default editor factory:
set datatype [installDataType ircam.jmax.fts.FtsPatchDataType ircam.jmax.editors.ermes.ErmesPatcherFactory]
installDataType ircam.jmax.fts.FtsGroupDataType ircam.jmax.editors.ermes.ErmesPatcherFactory
# patcher data type handlers:
installDataHandler ircam.jmax.mda.MaxTclFileDataHandler
installDataHandler ircam.jmax.fts.FtsDotPatFileDataHandler
installDataHandler ircam.jmax.mda.MaxTclStringDataHandler

##other types here...


