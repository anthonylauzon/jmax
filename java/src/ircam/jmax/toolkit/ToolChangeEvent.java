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
