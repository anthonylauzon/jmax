
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;
import javax.swing.*;

/**
 * The base class of the tools in the Table package. */
abstract public class TableTool extends Tool {

  public TableTool(String theName, ImageIcon theIcon)
  {
    super(theName, theIcon);
  }

  /**
   * Returns the Tool's GraphicContext as a TableGraphicContext */
  public TableGraphicContext getGc()
  {
    return (TableGraphicContext) gc;
  }
}
