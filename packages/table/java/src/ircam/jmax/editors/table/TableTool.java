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

import ircam.jmax.toolkit.*;
import javax.swing.*;

/**
 * The base class of the tools in the Table package. */
abstract public class TableTool extends Tool {

  public TableTool(String theName, ImageIcon theIcon)
  {
    super(theName, theIcon);
  }

  /**
   * Returns the Tool's GraphicContext as a TableGraphicContext */
  public TableGraphicContext getGc()
  {
    return (TableGraphicContext) gc;
  }
}

