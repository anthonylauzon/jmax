//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
