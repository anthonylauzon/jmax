package ircam.jmax.toolkit;

import java.util.*;

/**
 * The interface of the objects that listen to ToolChange events
 * (for example, the ones rised by a toolbar)
 */
public interface ToolListener extends EventListener{
  /**
   * the single callback
   */
  abstract public void toolChanged(ToolChangeEvent newTool);

}
