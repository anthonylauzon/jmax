
package ircam.jmax.editors.qlist;

import tcl.lang.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The qlist extension; install the qlist data type
 * and the qlist file data handler
 */

public class QListExtension extends tcl.lang.Extension
{
  public void init(Interp interp)
  {
    Mda.installEditorFactory(new QListFactory());
  }
}



