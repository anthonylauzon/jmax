package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * an interaction module used to resize a selection of objects.
 * At the end of a resize operation, it communicates the new position to the listener
 */
public class SelectionResizer extends InteractionModule implements XORPainter {

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
    int deltaX = gc.getAdapter().getInvX(e.getX()) - gc.getAdapter().getInvX(itsStartingPoint.x);
    int deltaY = gc.getAdapter().getInvY(e.getY()) - gc.getAdapter().getInvY(itsStartingPoint.y);
    
    gc.getStatusBar().post(ScrToolbar.getTool(), " delta x "+(deltaX)+"    delta y"+deltaY);

    itsXORHandler.moveTo(e.getX(), e.getY());
  }

  /**
   * overrides InteractionModule.mouseReleased()
   */
  public void mouseReleased(MouseEvent e) 
  {
    itsXORHandler.end();

    itsListener.dragEnd(e.getX(), 0);
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
  public void XORDraw(int dx, int dy) 
  {
    previousX = dx;
    previousY = dy;

    Graphics g = gc.getGraphicDestination().getGraphics();
    g.setColor(Color.darkGray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.
    
    ScrEvent aScrEvent;
    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aScrEvent = (ScrEvent) e.nextElement();
	gc.getAdapter().setX(tempEvent, gc.getAdapter().getX(aScrEvent));
	gc.getAdapter().setY(tempEvent, gc.getAdapter().getY(aScrEvent));

	gc.getAdapter().setLenght(tempEvent, gc.getAdapter().getLenght(aScrEvent)+dx);
	gc.getRenderer().getEventRenderer().render(tempEvent, g, false);
      }
	 
    g.setPaintMode();
    g.setColor(Color.black);
  }

  //---- Fields
  DragListener itsListener;
  XORHandler itsXORHandler;  
  int previousX;
  int previousY;

  Rectangle enclosingRect = new Rectangle();

  Point itsStartingPoint;
  int itsMovements;

  ScrEvent tempEvent = new ScrEvent(null);
}
