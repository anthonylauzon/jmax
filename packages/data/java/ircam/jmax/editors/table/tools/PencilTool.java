//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.editors.table.tools;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.table.*;

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

    setCursor(theImageIcon, new Point(4, 1));
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
  public void dragStart(int x, int y, MouseEvent e)
  {
    getGc().getFtsObject().beginUpdate();
    previousX = x;
    previousY = y;
    setPoint(x, y);
  }

  /**
   * DynamicDragListener interface */
  public void dynamicDrag(int x, int y, MouseEvent e)
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

    getGc().getFtsObject().interpolate(start, end, ta.getInvY(y1), ta.getInvY(y2));

    previousX = x;
    previousY = y;
  }
  
  /**
   * DynamicDragListener interface */
  public void dragEnd(int x, int y, MouseEvent e)
  {
    setPoint(x, y);
    getGc().getFtsObject().requestEndEdit();
  }

  public void updateStartingPoint(int deltaX, int deltaY){}

  /** set the value of a point in the model */
  private void setPoint(int x, int y)
  {
    getGc().getFtsObject().requestSetValue(getGc().getAdapter().getInvX(x), getGc().getAdapter().getInvY(y));
  }

  //-------------- Fields

  FreeHandDrawer itsFreeHand;
  int previousX;
  int previousY;
  Cursor cursor;
}



