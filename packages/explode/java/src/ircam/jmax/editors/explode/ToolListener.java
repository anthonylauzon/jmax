package ircam.jmax.editors.explode;

import java.util.*;

/**
 * The interface of the objects that listen to a toolbar
 */
public interface ToolListener extends EventListener{
  abstract public void toolChanged(ToolChangeEvent newTool);

}
