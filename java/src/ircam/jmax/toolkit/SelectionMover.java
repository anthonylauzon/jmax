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
    InteractionSemaphore.lock();
  }

  /**
   * overrides InteractionModule.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    itsXORHandler.moveTo(e.getX(), e.getY());
  }

  public void moveTo(int x, int y)
  {
    itsXORHandler.moveTo(x, y);
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
    InteractionSemaphore.unlock();
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
