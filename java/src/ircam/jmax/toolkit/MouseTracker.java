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









