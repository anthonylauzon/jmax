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

import java.awt.event.*;

/**
 * A simple interaction module to detect the direction of movement
 * of the mouse during a drag, and to  communicate it to a DirectionListener.
 * This class does not handle mouseDown, so it must be mounted
 * when a mouseDown already happened.
 * Warning: this module continuosly send direction informations during 
 * mouseDrags, so do not left it active more then needed.
 */
public class DirectionChooser extends InteractionModule {
 

  /**
   * constructor with the direction listener
   */
  public DirectionChooser(DirectionListener theListener)
  {
    super();
    itsListener = theListener;
  } 


  /**
   * sets the point to be considered as origin of the movement
   */
  public void interactionBeginAt(int x, int y)
  {
    itsX = x;
    itsY = y;
  }

  //Mouse interface
  
  public void mouseDragged(MouseEvent e) 
  {  
    if (Math.abs(e.getX()-itsX) > Math.abs(e.getY()-itsY))    
	itsListener.directionChoosen(HORIZONTAL_MOVEMENT);
	else itsListener.directionChoosen(VERTICAL_MOVEMENT);
  } 

  public void mouseReleased(MouseEvent e)
  {
    itsListener.directionAbort();
  }
 
  //--- Fields
  DirectionListener itsListener;
  int itsX;
  int itsY;

  public static final int HORIZONTAL_MOVEMENT = 1;
  public static final int VERTICAL_MOVEMENT = 2;
}
