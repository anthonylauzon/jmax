
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

/** The base class for the interaction modules used in Table.
 * It eliminates the need to cast the Tool's GraphicContext to
 * TableGraphicContext.
 * @see InteractionModule
 */
public class TableInteractionModule extends InteractionModule{

  /**
   * Returns the GraphicContext of this tool as TableGraphicContext */
  public TableGraphicContext getGc()
  {
    return (TableGraphicContext) gc;
  }
}
