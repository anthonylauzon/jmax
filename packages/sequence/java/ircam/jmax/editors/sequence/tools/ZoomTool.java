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

package ircam.jmax.editors.sequence.tools;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track. *;

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;

/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ZoomTool extends Tool implements  DirectionListener, DynamicDragListener {

  /** constructor */
  public ZoomTool(ImageIcon theImageIcon) 
  {
    super("zoomer", theImageIcon);

    itsDirectionChooser = new DirectionChooser(this);
    itsMouseTracker = new MouseDragTracker(this);
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
   * DynamicDragListener interface
   */
  public void dragStart(int x, int y, MouseEvent e)
  {
    ((SequenceGraphicContext)gc).getTrack().setProperty("selected", Boolean.TRUE);
    
    /* workaround: right-mouse events used when popup is visible */	
    if(((SequenceGraphicContext)gc).getTrackEditor().getMenu().isVisible()) return;
    
    mountIModule(itsDirectionChooser, x, y);
    tempX = x;
    tempY = y;
  }
  public void dynamicDrag(int deltaX, int deltaY, MouseEvent e)
  {
    /* workaround: right-mouse events used when popup is visible */	
    if(((SequenceGraphicContext)gc).getTrackEditor().getMenu().isVisible()) return;
    
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
    Geometry geometry = egc.getAdapter().getGeometry();
    if(egc.getAdapter().isVerticalZoomable())
      {
	if((direction & SelectionMover.HORIZONTAL_MOVEMENT) != 0)
	  geometry.incrXZoom(deltaX);
	else
	  if((direction & SelectionMover.VERTICAL_MOVEMENT) != 0)
	    {
	      //vertical zoom
	      ((VerticalZoomable)egc.getAdapter()).incrYZoom(deltaY);
	      egc.getGraphicDestination().repaint();
	    }
      }
    else
      geometry.incrXZoom(deltaX);
  }
  public void dragEnd(int x, int y, MouseEvent e){}
  public void updateStartingPoint(int deltaX, int deltaY){}
  
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

  //-------------- Fields
  MouseDragTracker itsMouseTracker;
  DirectionChooser itsDirectionChooser;
  int tempX = 0;
  int tempY = 0;
  int direction;
}



