
package ircam.jmax.editors.explode;

import java.util.*;

/**
 * the interface of the objects that can provide tools to a toolbar.
 */
public interface ToolbarProvider {

  /**
   * returns the collection of tools to be inserted in the toolbar
   */
  abstract public Enumeration getTools();
}
