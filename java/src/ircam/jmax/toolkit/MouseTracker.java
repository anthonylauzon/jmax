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

package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;


/**
 * a simple interaction module:
 * it just waits the choice of a point in a graphicContext via mouse-clicks.
 * It communicates the choice to a "Position listener"
 */
public class MouseTracker extends InteractionModule {

  /**
   * constructor.
   */
  public MouseTracker(PositionListener theListener) 
  {
    super();
    itsListener = theListener;
  }


  //Mouse interface
  
  public void mousePressed(MouseEvent e) 
  {  
    itsListener.positionChoosen(e.getX(), e.getY(), e.getModifiers());
  } 

  public void interactionBeginAt(int x, int y)
  {
    //useless for this kind of interaction module
  }

  //------------- fields
  PositionListener itsListener;

}









