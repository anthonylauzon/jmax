//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.editors.table;

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;
import javax.swing.undo.*;

/**
 * The class implementing the behaviour of the pencil in the Table editor.
 * It uses a FreeHandDrawer Interaction Module
 * @see TableTool
 */ 
public class PencilTool extends TableTool implements DynamicDragListener {

  /**
   * Constructor.
   */
  public PencilTool(ImageIcon theImageIcon) 
  {
    super("pencil", theImageIcon);

    itsFreeHand = new FreeHandDrawer(this);
  }


  /**
   * Redefined to deselect all at activation time */
  public void reActivate(GraphicContext gc) 
  {
    super.reActivate(gc);
    getGc().getSelection().deselectAll();
  }

  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsFreeHand;
  }


  /**
   * DynamicDragListener interface */
  public void dragStart(int x, int y)
  {
    getGc().getDataModel().beginUpdate();
    previousX = x;
    previousY = y;
    setPoint(x, y);
  }

  /**
   * DynamicDragListener interface */
  public void dynamicDrag(int x, int y)
  {
    TableAdapter ta = getGc().getAdapter();
    int x1 = previousX;
    int y1 = previousY;
    int x2 = x;
    int y2 = y;

    if (x1 > x2) //the line is given 'a l'inverse'
      {
	int temp;
	temp = y1;
	y1 = y2;
	y2 = temp;
	temp = x1;
	x1 = x2;
	x2 = temp;
      }

    int start = ta.getInvX(x1); 
    int end = ta.getInvX(x2);

    getGc().getDataModel().interpolate(start, end, ta.getInvY(y1), ta.getInvY(y2));

    previousX = x;
    previousY = y;
  }
  
  /**
   * DynamicDragListener interface */
  public void dragEnd(int x, int y)
  {
    setPoint(x, y);
    getGc().getDataModel().endUpdate();
  }

  /** set the value of a point in the model */
  private void setPoint(int x, int y)
  {
    getGc().getDataModel().setValue(getGc().getAdapter().getInvX(x), getGc().getAdapter().getInvY(y));
  }

  //-------------- Fields

  FreeHandDrawer itsFreeHand;
  int previousX;
  int previousY;
}



