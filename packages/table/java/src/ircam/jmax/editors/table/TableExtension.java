
package ircam.jmax.editors.table;

import tcl.lang.*;
import ircam.jmax.fts.*;
import ircam.jmax.mda.*;

/**
 * The table extension; install the table data type
 * and the table file data handler
 */

public class TableExtension extends tcl.lang.Extension
{
  public void init(Interp interp)
  {
    MaxDataType.getTypeByName("IntegerVector").setDefaultEditorFactory(new TableFactory());
  }
}



