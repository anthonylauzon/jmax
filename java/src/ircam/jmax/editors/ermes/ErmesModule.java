package ircam.jmax.editors.ermes;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;

/** The ermes module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class ErmesModule
{
  static public void initModule()
  {
    // Install the local mda entities

    MaxDataType.getTypeByName("patcher").setDefaultEditorFactory(new ErmesPatcherFactory());

    // Install the module tcl commands.

    ircam.jmax.editors.ermes.tcl.TclErmesPackage.installPackage();

    //List of persistent properties

    FtsPropertyDescriptor.setPersistent("autorouting", true);    
    FtsPropertyDescriptor.setDefaultValue("autorouting", "on");
  }
}
