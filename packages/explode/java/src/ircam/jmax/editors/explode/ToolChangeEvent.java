package ircam.jmax.editors.explode;

import java.util.*;

/**
 * the event rised by toolbars when the user change tool.
 */
public class ToolChangeEvent extends EventObject {
  ScrTool itsTool;

  /**
   * constructor
   */
  public ToolChangeEvent(ScrTool theTool) {
    
    super(theTool);
    itsTool = theTool;
  
  }

  public void setTool(ScrTool theTool) {
    itsTool = theTool;
  }

  public ScrTool getTool() {
    return itsTool;
  }
}
