//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.table.tools;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;
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

