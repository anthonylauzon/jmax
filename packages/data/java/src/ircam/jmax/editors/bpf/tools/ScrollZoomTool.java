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
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.editors.bpf.tools;

import ircam.jmax.editors.bpf.*;

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;

/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ScrollZoomTool extends Tool implements  DirectionListener, DynamicDragListener {

  /** constructor */
  public ScrollZoomTool(ImageIcon theImageIcon) 
  {
    super("zoom&scroll tool", theImageIcon);

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

  public Cursor getCursor()
  {
    return Cursor.getPredefinedCursor(Cursor.MOVE_CURSOR);
  }  
  
  /**
   * DynamicDragListener interface
   */
    public void dragStart(int x, int y, MouseEvent e)
    {
	/* workaround: right-mouse events used when popup is visible */
	if(((BpfEditor)gc.getGraphicSource()).getMenu().isVisible()) return;

	if(e.getClickCount() > 1) 
	    {
		Geometry g = ((BpfGraphicContext)gc).getAdapter().getGeometry();
		g.setXZoom(20);
		g.setXTransposition(0);
	    }

	mountIModule(itsDirectionChooser, x, y);
	tempX = x;
	tempY = y;
    }
    private boolean rep = false;
    public void dynamicDrag(int deltaX, int deltaY, MouseEvent e)
    {
	/* workaround: right-mouse events used when popup is visible */
	if(((BpfEditor)gc.getGraphicSource()).getMenu().isVisible()) return;
	BpfGraphicContext bgc = (BpfGraphicContext) gc;
	Geometry geometry = bgc.getAdapter().getGeometry();

	if(e.isShiftDown())
	    {
		int transp = geometry.getXTransposition()+bgc.getAdapter().getInvWidth(deltaX);
		bgc.getScrollManager().scrollToValue(-transp);	    
	    }
	else
	    geometry.incrXZoom(deltaX);
    }
    public void dragEnd(int x, int y){}
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



