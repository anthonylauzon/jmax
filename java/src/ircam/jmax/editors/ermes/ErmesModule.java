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

    // Load the toolbar images

    ErmesSwToolbar.loadToolBarImages();

    // Register the find panel and the fpe panel

    FindPanel.registerFindPanel();
    FpePanel.registerFpePanel();
  }
}



