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

    Mda.installEditorFactory(new ErmesPatcherFactory());

    //List of persistent properties

    FtsPropertyDescriptor.setDefaultValue("autorouting", "off");
    FtsPropertyDescriptor.setDefaultValue("maxValue", new Integer(128));
    FtsPropertyDescriptor.setDefaultValue("minValue", new Integer(0));
    FtsPropertyDescriptor.setDefaultValue("resized", "off");

    // Load the toolbar images

    ErmesSwToolbar.loadToolBarImages();
  }
}



