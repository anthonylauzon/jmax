
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import com.sun.java.swing.*;

/**
 * The table selecter. It uses the StripeSelecter Interaction Module.
 * @see StripeSelecter.
 */
public class TableSelecter extends TableTool implements GraphicSelectionListener{
  
  public TableSelecter(ImageIcon theImageIcon)
  {
    super("selecter", theImageIcon);

    itsSelecter = new StripeSelecter(this);
  }

  /**
   * the default InteractionModule for this kind of tools
   */
  public InteractionModule getDefaultIM() 
  {
    return itsSelecter;
  }

  /**
   * called by the Selecter UI module at mouse down
   */
  public void selectionPointChoosen(int x, int y, int modifiers) 
  {

    if ((modifiers & InputEvent.SHIFT_MASK) == 0)
      {
	TableSelection.getSelection().deselectAll();
	
	gc.getFrame().repaint();
      }
  }

  /**
   * called by the selecter UI module at mouseUp.
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    TableAdapter a = getGc().getAdapter();
    TableSelection.getSelection().select(new Point(a.getInvX(x), a.getInvX(x+w)));
  }

  /**
   * Selects all the objects in a given rectangle
   */
  void selectArea(int x, int y, int w, int h) 
  { 
  }

  
  //--- Fields
  StripeSelecter itsSelecter;

  Point startingPoint = new Point();

}

