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

package ircam.jmax.toolkit;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * An abstract interaction module used to resize a selection of objects.
 * At the end of a resize operation, it communicates the new position to the listener.
 * Just redefine the XORDraw method in order to specify what to draw during the resize.
 * @see DragListener
 * @see InteractionModule 
 */
abstract public class SelectionResizer extends InteractionModule implements XORPainter {

  /**
   * constructor. 
   * It uses a XORHandler to keep track of the drawing positions.
   */
  public SelectionResizer(DragListener theListener) 
  {
    super();
    
    itsListener = theListener;
    itsXORHandler = new XORHandler(this, XORHandler.ABSOLUTE);
    itsStartingPoint = new Point();

    movement = HORIZONTAL_MOVEMENT;
  }

  /**
   * set the direction of movement (constants HORIZONTAL_MOVEMENT  or 
   * VERTICAL_MOVEMENT, or both)
   */
  public void setDirection(int theDirection) 
  {
    movement = theDirection;
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

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    itsXORHandler.end();

    int endX = itsStartingPoint.x;
    int endY = itsStartingPoint.y;

    if ( movement == HORIZONTAL_MOVEMENT)
      endX = e.getX();
    else     
      if ( movement == VERTICAL_MOVEMENT)
	endY = e.getY();
    
    itsListener.dragEnd( endX, endY, e);
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
   * from the XORPainter interface. The actual drawing function.
   */
  abstract public void XORDraw(int dx, int dy);


  //---- Fields
  protected DragListener itsListener;
  protected XORHandler itsXORHandler;  
  protected int previousX;
  protected int previousY;

  public Point itsStartingPoint;
  public static final int HORIZONTAL_MOVEMENT = 1;
  public static final int VERTICAL_MOVEMENT = 2; 
  protected int movement;
}
