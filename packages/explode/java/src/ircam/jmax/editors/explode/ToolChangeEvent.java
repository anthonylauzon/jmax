package ircam.jmax.editors.explode;

import java.util.*;

/**
 * the event rised by toolbars when the user change tool.
 */
public class ToolChangeEvent extends EventObject {

  /**
   * constructor
   */
  public ToolChangeEvent(ScrTool theTool) 
  {  
    super(theTool);
    itsTool = theTool;
  }

  /**
   * set the Tool associated to this event
   */
  public void setTool(ScrTool theTool) 
  {
    itsTool = theTool;
  }

  /**
   * get the Tool associated to this event
   */
  public ScrTool getTool() 
  {
    return itsTool;
  }

  //--- Fields
  ScrTool itsTool;

}
