
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import javax.swing.*;

public class TableCutter extends Tool implements GraphicSelectionListener {
  
  public TableCutter(ImageIcon theImageIcon)
  {
    super("cutter", theImageIcon);

    itsSelecter = new StripeSelecter(this);
  }
  /**
   * called by the Selecter UI module at mouse down
   */
  public void selectionPointChoosen(int x, int y, int modifiers) 
  {
  }

  /**
   * called by the selecter UI module
   */
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    int start = ((TableGraphicContext)gc).getAdapter().getInvX(x);
    int end = ((TableGraphicContext)gc).getAdapter().getInvX(x+w);

    cutAndShrink(start, end);

    TableSelection.getSelection().deselectAll();
    gc.getFrame().repaint();
  }


  private void cutAndShrink(int start, int end)
  {
    TableDataModel model = ((TableGraphicContext)gc).getDataModel();
    int toend = model.getSize()-end;
    
    for (int i = 0; i < toend-1; i++)
      {
	model.setValue(start+i, model.getValue(end+i));
      }
    
    for (int j = start+toend; j < model.getSize(); j++)
      {
	model.setValue(j, 0);
      }
    
  }

  public InteractionModule getDefaultIM()
  {
    return itsSelecter;
  }

  public void reActivate(GraphicContext gc)
  {
    super.reActivate(gc);
    Point temp;

    for (Enumeration e = TableSelection.getSelection().getSelected();
	 e.hasMoreElements();)
      {
	temp = (Point) e.nextElement();
	cutAndShrink(temp.x, temp.y);
      }
    
    TableSelection.getSelection().deselectAll();
    gc.getFrame().repaint();
  }

  //--- Fields
  StripeSelecter itsSelecter;
}





