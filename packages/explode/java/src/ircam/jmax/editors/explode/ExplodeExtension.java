
package ircam.jmax.editors.explode;

import tcl.lang.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The explode extension; install the explode data type,
 * register the remote data class, installs
 * the explode editor factory
 */
public class ExplodeExtension extends tcl.lang.Extension
{

  /**
   * overrides tcl.lang.Extension.init()
   */
  public void init(Interp interp)
  {
    Mda.installEditorFactory( new ExplodeFactory());

    Fts.registerRemoteDataClass( "explode_data", ircam.jmax.editors.explode.ExplodeRemoteData.class);
  }

}



