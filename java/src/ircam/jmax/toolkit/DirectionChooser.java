//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
