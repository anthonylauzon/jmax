package ircam.jmax.editors.explode;

import java.util.*;

/**
 * The interface of the objects that listen to ToolChange events
 * (example rised by a toolbar)
 */
public interface ToolListener extends EventListener{
  /**
   * the callback
   */
  abstract public void toolChanged(ToolChangeEvent newTool);

}
