
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
  public void init(Interp interp)
  {
    Mda.installEditorFactory( new ExplodeFactory());

    FtsObject.registerRemoteDataObjectClass( "explode");
  }
}



