package ircam.jmax.editors.ermes;

import ircam.jmax.mda.*;

/** The ermes module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class ErmesModule
{
  static public void initModule()
  {
    // Install the local mda entities

    MaxDataType.getTypeByName("patch").setDefaultEditorFactory(new ErmesPatcherFactory());

    // Install the module tcl commands.

    ircam.jmax.editors.ermes.tcl.TclErmesPackage.installPackage();
  }
}
