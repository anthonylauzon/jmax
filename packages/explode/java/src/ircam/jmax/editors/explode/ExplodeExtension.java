
package ircam.jmax.editors.explode;

import tcl.lang.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The explode extension; install the explode data type
 * and the explode file data handler
 */

public class ExplodeExtension extends tcl.lang.Extension
{
  ExplodeRemoteData data;

  public void init(Interp interp)
  {
    Mda.installEditorFactory( new ExplodeFactory());

    Fts.registerRemoteDataClass( "explode_data", ircam.jmax.editors.explode.ExplodeRemoteData.class);
  }
}



