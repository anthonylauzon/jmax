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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;

/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ZoomTool extends Tool implements PositionListener {

  /** constructor */
  public ZoomTool(ImageIcon theImageIcon) 
  {
    super("zoomer", theImageIcon);

    itsMouseTracker = new MouseTracker(this);
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
   * PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
 
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    if ((modifiers & InputEvent.ALT_MASK) == 0) //zoom out
      {
	egc.getAdapter().setXZoom((int) (egc.getAdapter().getXZoom() * 100 * 2));

      }
    else 
      {                                       //zoom in
	if (egc.getAdapter().getXZoom() <= 0.01) return;
	egc.getAdapter().setXZoom((int) (egc.getAdapter().getXZoom() * 100 / 2));

      }

    //gc.getGraphicDestination().repaint();
  }
  

  //-------------- Fields

  MouseTracker itsMouseTracker;
}



