//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
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
import java.util.*;
import java.awt.event.*;

/**
 * An abstract interaction module used to move a selection of objects.
 * At the end of a move operation, it communicates the new position to the listener.
 * Just redefine the XORDraw method to specify what to draw during drag.
 * @see DragListener
 */
abstract public class SelectionMover extends InteractionModule implements XORPainter {

  /**
   * constructor. 
   * It uses a XORHandler to keep track of the drawing positions.
   */
  public SelectionMover(DragListener theListener, int theMovement) 
  {
    super();
    
    itsListener = theListener;
    itsXORHandler = new XORHandler(this, XORHandler.ABSOLUTE);
    itsStartingPoint = new Point();

    itsMovements = theMovement;
  }
  

  /**
   * set the direction of movement (constants HORIZONTAL_MOVEMENT  or 
   * VERTICAL_MOVEMENT, or both)
   */
  public void setDirection(int theDirection) 
  {
    itsMovements = theDirection;
  }

  /**
   * sets the point on which to start the movement
   */
  public void interactionBeginAt(int x, int y) 
  {
    itsStartingPoint.setLocation(x, y);
    itsXORHandler.beginAt(x, y);
  }

  /**
   * overrides InteractionModule.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    itsXORHandler.moveTo(e.getX(), e.getY());
  }

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    itsXORHandler.end();

    int endX = itsStartingPoint.x;
    int endY = itsStartingPoint.y;

    if ((itsMovements & VERTICAL_MOVEMENT) != 0)
      endY = e.getY();
    if ((itsMovements & HORIZONTAL_MOVEMENT) != 0)
      endX = e.getX();
    
    itsListener.dragEnd(endX, endY);
  }


  /**
   * from the XORPainter interface
   */
  public void XORErase() 
  {
    XORDraw(previousX, previousY);
  }


  /**
   * From the XORPainter interface. The actual drawing function.
   */
  abstract public void XORDraw(int dx, int dy);



  //---- Fields
  DragListener itsListener;
  XORHandler itsXORHandler;  
  protected int previousX;
  protected int previousY;

  protected Point itsStartingPoint;
  protected int itsMovements;

  public int dragMode;
  public static final int RECT_DRAG = 0;
  public static final int GROUP_DRAG = 1;
  public static final int HORIZONTAL_MOVEMENT = 1;
  public static final int VERTICAL_MOVEMENT = 2;

}
