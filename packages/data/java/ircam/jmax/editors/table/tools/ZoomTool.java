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

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;
import ircam.jmax.editors.table.*;
/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ZoomTool extends TableTool implements  DirectionListener, TableDynamicDragListener, GraphicSelectionListener{

  /** constructor */
  public ZoomTool(ImageIcon theImageIcon) 
  {
    super("zoomer", theImageIcon);

    setCursor( theImageIcon, new Point(5, 5));
    itsDirectionChooser = new DirectionChooser(this);
    itsMouseTracker = new TableMouseDragTracker(this);
    itsSelecter = new StripeSelecter(this);
  }

  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }

  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }

    /**
     * Redefined to deselect all at activation time */
  public void reActivate(GraphicContext gc) 
  {
    super.reActivate(gc);
    getGc().getSelection().deselectAll();
  }
  
  /**
   * DynamicDragListener interface
   */
  public void dragStart(int x, int y, MouseEvent e)
  {
    /* workaround: right-mouse events used when popup is visible */
    if(((TableDisplay)gc.getGraphicSource()).getMenu().isVisible()) return;
  
    if( e.isShiftDown())
      mountIModule(itsSelecter, x, y);
    else
      mountIModule(itsDirectionChooser, x, y);

    tempX = x;
    tempY = y;
    
    dddx = 0;
    dddy = 0;
  }
  
  int dddx = 0;
  int dddy = 0;  
  public void dynamicDrag(int deltaX, int deltaY, MouseEvent e)
  {
    /* workaround: right-mouse events used when popup is visible */
    if(((TableDisplay)gc.getGraphicSource()).getMenu().isVisible()) return;

    if( !e.isShiftDown())
    {
        TableAdapter a = ((TableGraphicContext)gc).getAdapter();
        if((direction & SelectionMover.HORIZONTAL_MOVEMENT) != 0)
        {
            float xZoom = a.getXZoom();
            dddx+=deltaX;
            if(dddx>35)
            {
                if (xZoom>=0.9)
                    a.setXZoom(Math.round(xZoom)+1);
                else
                    a.setXZoom(xZoom*(1/(1-xZoom)));
	    
                dddx=0;
            }
            else if(dddx<-35)
            {
                if (xZoom>1.9) 
                    a.setXZoom(Math.round(xZoom)-1);
                else
                a.setXZoom(xZoom*(1/(1+xZoom)));
                dddx=0;
            }		    
        }
        else
        if((direction & SelectionMover.VERTICAL_MOVEMENT) != 0)
            a.incrYZoom( -deltaY);
    }
  }	
  public void dragEnd(int x, int y, MouseEvent e){}
  public void updateStartingPoint(int deltaX, int deltaY){}

  public void doubleClick( MouseEvent e)
  {
    if( e.isShiftDown())
        (((TableGraphicContext)gc).getAdapter()).zoomToWindow();
    else	
        (((TableGraphicContext)gc).getAdapter()).setDefaultZooms();
  }
  
  /**
   * DirectionListener interface
   */
  public void directionChoosen(int theDirection) 
  {
    direction = theDirection;
    mountIModule(itsMouseTracker, tempX , tempY);
  }
  public void directionAbort()
  {
    mountIModule(itsMouseTracker, tempX , tempY);
  }

  /**
   * GraphicSelectionListener interface
   */
  public void selectionPointChoosen(int x, int y, int modifiers){}
  public void selectionChoosen(int x, int y, int w, int h) 
  {
    if( w != 0)
    {
        TableAdapter a = ((TableGraphicContext)gc).getAdapter();
        Dimension size = gc.getGraphicDestination().getSize();
        int X =  a.getInvX(x);

        double width = Math.abs( X - a.getInvX(x+w));            
        a.setXZoom( a.findZoomRatioClosestTo((float)(size.width/width)));
        ((TableGraphicContext)gc).scrollTo( X);
    }
    mountIModule(itsMouseTracker, tempX , tempY);
  }
  public void selectionPointDoubleClicked(int x, int y, int modifiers){}
  //-------------- Fields
  TableMouseDragTracker itsMouseTracker;
  DirectionChooser itsDirectionChooser;
  StripeSelecter itsSelecter;

  int tempX = 0;
  int tempY = 0;
  int direction;
  Cursor cursor;
}




