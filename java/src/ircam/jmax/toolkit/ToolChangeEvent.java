//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.toolkit;

import java.util.*;

/**
 * The event rised by toolbars when the user change tool.
 */
public class ToolChangeEvent extends EventObject {

  /**
   * constructor
   */
  public ToolChangeEvent(Tool theTool) 
  {  
    super(theTool);
    itsTool = theTool;
  }

  /**
   * set the Tool associated to this event
   */
  public void setTool(Tool theTool) 
  {
    itsTool = theTool;
  }

  /**
   * get the Tool associated to this event
   */
  public Tool getTool() 
  {
    return itsTool;
  }

  //--- Fields
  Tool itsTool;

}


