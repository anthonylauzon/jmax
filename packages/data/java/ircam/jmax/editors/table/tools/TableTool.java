//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
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
import javax.swing.*;

/**
 * The class implementing the behaviour of the pencil in the Table editor.
 * It uses a FreeHandDrawer Interaction Module
 * @see TableTool
 */ 
public class TableTool extends Tool implements DynamicDragListener, GraphicSelectionListener, LineListener
{

  /**
   * Constructor.
   */
  public TableTool(ImageIcon theImageIcon) 
  {
    super("edit", theImageIcon);

    itsFreeHand = new FreeHandDrawer(this);
    itsSelecter = new StripeSelecter(this);
    itsLiner = new LineDrawer(this);
  }

  /**
   * Redefined to deselect all at activation time */
  public void reActivate(GraphicContext gc) 
  {
    super.reActivate(gc);
    ((TableGraphicContext)gc).getSelection().deselectAll();
  }

  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsSelecter;
  }
  
/********************** DynamicDragListener interface ******************************/
  public void dragStart(int x, int y, MouseEvent e)
  {
    ((TableGraphicContext)gc).getFtsObject().beginUpdate();
    previousX = x;
    previousY = y;
    setPoint(x, y);
  }

  public void dynamicDrag(int x, int y, MouseEvent e)
  {
    TableAdapter ta = ((TableGraphicContext)gc).getAdapter();
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

    double startY = ta.getInvY(y1);
    double endY = ta.getInvY(y2);
    float max = ((TableGraphicContext)gc).getVerticalMaxValue();
    float min = ((TableGraphicContext)gc).getVerticalMinValue();
    if( startY >  max) startY = (double)max;
    else if( startY < min) startY = (double)min;
    if( endY >  max) endY = (double)max;
    else if( endY < min) endY = (double)min;

    ((TableGraphicContext)gc).getFtsObject().interpolate(start, end, startY, endY);

    previousX = x;
    previousY = y;
  }
  public void dragEnd(int x, int y, MouseEvent e)
  {
    setPoint(x, y);
    ((TableGraphicContext)gc).getFtsObject().requestEndEdit();
  
    reActivate(gc);
  }

  public void updateStartingPoint(int deltaX, int deltaY){}

  /** set the value of a point in the model */
  private void setPoint(int x, int y)
  {
    double Y = ((TableGraphicContext)gc).getAdapter().getInvY(y);
    float max = ((TableGraphicContext)gc).getVerticalMaxValue();
    float min = ((TableGraphicContext)gc).getVerticalMinValue();
    if( Y >  max) Y = (double)max;
    else if( Y < min) Y = (double)min;
  
    ((TableGraphicContext)gc).getFtsObject().requestSetValue(((TableGraphicContext)gc).getAdapter().getInvX(x), Y);
  }
  
  /********************************************************************************************/
  /********************************** GraphicSelectionListener *****************************************************/
  public void selectionPointChoosen(int x, int y, MouseEvent e) 
  {
    int modifiers = e.getModifiers();
    
    if((modifiers & Toolkit.getDefaultToolkit().getMenuShortcutKeyMask()) != 0) /* CMD is down: do edit */
    {
      if((modifiers & MouseEvent.ALT_MASK) == 0)/* ALT is down: line edit */
      {
        ((TableGraphicContext)gc).getSelection().deselectAll();
        itsFreeHand.activate(e);
        mountIModule(itsFreeHand, x, y);   
      }
      else /* ALT is down: free edit */
      {
        ((TableGraphicContext)gc).getSelection().deselectAll();
        itsLiner.activate(e);
        mountIModule(itsLiner, x, y);     
      }
    }
    else
    {
      if ((modifiers & InputEvent.SHIFT_MASK) == 0)
      {
        ((TableGraphicContext)gc).getSelection().deselectAll();
        ((TableGraphicContext)gc).getSelection().setCaretPosition(((TableGraphicContext)gc).getAdapter().getInvX(x));
      }
      gc.getGraphicDestination().requestFocus();
    }
  }
  
  public void selectionChoosen(int x, int y, int w, int h, int modifiers) 
  {
    TableAdapter a = ((TableGraphicContext)gc).getAdapter();
  
    if (w==0) return;
    
    ((TableGraphicContext)gc).getSelection().setCaretPosition(TableSelection.NO_CARET);
    int start = (a.getInvX(x)>0) ? a.getInvX(x) : 0;
    int end = (a.getInvX(x+w) < ((TableGraphicContext)gc).getFtsObject().getSize()) ? a.getInvX(x+w) : (((TableGraphicContext)gc).getFtsObject().getSize());
    ((TableGraphicContext)gc).getSelection().select(start, end);
    
    gc.getGraphicDestination().requestFocus();
  }
  public void selectionPointDoubleClicked(int x, int y, int modifiers){}
  /********************************************************************************************/
  /********************************** LineListener *****************************************************/
  public void lineStart(int x, int y, int modifiers) 
  {
    ((TableGraphicContext)gc).getFtsObject().beginUpdate();
    startingPoint.setLocation(x, y);
  }
  public void lineEnd(int x, int y) 
  {
    drawLine(startingPoint.x, startingPoint.y, x, y);
    ((TableGraphicContext)gc).getFtsObject().requestEndEdit();
    ((TableGraphicContext)gc).displayInfo("");
    
    reActivate(gc);
  }
  public void drawLine(int x1, int y1, int x2, int y2)
  {
    TableAdapter ta = ((TableGraphicContext)gc).getAdapter();
  
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
    
    ((TableGraphicContext)gc).getFtsObject().interpolate(start, end, ta.getInvY(y1), ta.getInvY(y2));
  }
  /********************************************************************************************/
  //-------------- fields drawer -----------------------/
  FreeHandDrawer itsFreeHand;
  int previousX;
  int previousY;
  Cursor cursor;
  //---------- fields selecter ---------------/
  StripeSelecter itsSelecter;
  Point startingPoint = new Point();
  //--- liner fields ----------/
  LineDrawer itsLiner;
}



