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
    /*int deltaX = gc.getAdapter().getInvX(e.getX()) - gc.getAdapter().getInvX(itsStartingPoint.x);
      int deltaY = gc.getAdapter().getInvY(e.getY()) - gc.getAdapter().getInvY(itsStartingPoint.y);
      
      gc.getStatusBar().post(ScrToolbar.getTool(), " dx "+deltaX);*/

    itsXORHandler.moveTo(e.getX(), e.getY());
  }

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    itsXORHandler.end();

    itsListener.dragEnd(e.getX(), 0); //this 0 is not generic!
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
  DragListener itsListener;
  XORHandler itsXORHandler;  
  protected int previousX;
  protected int previousY;

  public Point itsStartingPoint;

}
