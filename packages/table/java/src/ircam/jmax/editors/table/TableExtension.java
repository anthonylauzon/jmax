//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//

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
    Mda.installEditorFactory(new TableFactory());

  }
}



