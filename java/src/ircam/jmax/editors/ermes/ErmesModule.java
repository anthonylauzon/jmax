package ircam.jmax.editors.ermes;

import ircam.jmax.mda.*;
import ircam.jmax.fts.*;
import ircam.jmax.toolkit.*;

/** The ermes module; the init function is called at init time
 *  by jmax, and install module related things
 */

public class ErmesModule
{
  static public void initModule()
  {
    // Install the local mda entities

    Mda.installEditorFactory(new ErmesPatcherFactory());

    // Preload the icons
    IconCache.getIcon( "tool_ext.gif");
    IconCache.getIcon( "tool_patcher.gif");
    IconCache.getIcon( "tool_in.gif");
    IconCache.getIcon( "tool_out.gif");
    IconCache.getIcon( "tool_text.gif");
    IconCache.getIcon( "tool_mess.gif");
    IconCache.getIcon( "tool_bang.gif");
    IconCache.getIcon( "tool_toggle.gif");
    IconCache.getIcon( "tool_slider.gif");
    IconCache.getIcon( "tool_int.gif");
    IconCache.getIcon( "tool_float.gif");

    // Register the find panel and the fpe panel
    FindPanel.registerFindPanel();
    FpePanel.registerFpePanel();
  }
}



