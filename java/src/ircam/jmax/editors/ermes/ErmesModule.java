package ircam.jmax.editors.frobber;

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

    FtsPropertyDescriptor.setDefaultValue("fs", new Integer(10));
    FtsPropertyDescriptor.setDefaultValue("maxValue", new Integer(128));
    FtsPropertyDescriptor.setDefaultValue("minValue", new Integer(0));
    FtsPropertyDescriptor.setDefaultValue("resized", "off");

    // Load the toolbar images

    ErmesSwToolbar.loadToolBarImages();

    // Register the find panel

    FindPanel.registerFindPanel();
  }
}



