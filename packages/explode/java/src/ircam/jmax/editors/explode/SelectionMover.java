package ircam.jmax.editors.explode;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * an interaction module used to move a selection of objects.
 * At the end of a move operation, it communicates the new position to the listener
 */
public class SelectionMover extends InteractionModule implements XORPainter {

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
    computeEnclosure(enclosingRect);

    itsXORHandler.beginAt(x, y);
  }

  /**
   * computes the rectangle that surrounds the selection
   * and sets the given rectangle.
   */
  void computeEnclosure(Rectangle destination) 
  {  
    ScrEvent min_x;
    ScrEvent min_y;
    ScrEvent max_x;
    ScrEvent max_y;
    
    ScrEvent aEvent;
    
    Enumeration e=ExplodeSelection.getSelection().getSelected();
    aEvent = (ScrEvent) e.nextElement();    
    
    if (aEvent == null) return; //empty selection...
    else 
      {
	min_x = aEvent;
	max_x = aEvent;
	min_y = aEvent;
	max_y = aEvent;
      }

    Adapter a = gc.getAdapter();
    
    for (; e.hasMoreElements();)
      {
	aEvent = (ScrEvent) e.nextElement();

	if (a.getX(aEvent) < a.getX(min_x)) 
	 min_x = aEvent;
     
	if (a.getY(aEvent) < a.getY(min_y)) 
	  min_y = aEvent;
	
	if (a.getX(aEvent)+a.getLenght(aEvent) > a.getX(max_x)+a.getLenght(max_x)) 
	  max_x = aEvent;
	
	if (a.getY(aEvent) > a.getY(max_y)) 
	  max_y = aEvent;
      }
    
    destination.setBounds(a.getX(min_x), a.getY(min_y),
			  a.getX(max_x)+a.getLenght(max_x)-a.getX(min_x),
			  a.getY(max_y)-a.getY(min_y)+10);
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

    int endX = itsStartingPoint.x;
    int endY = itsStartingPoint.y;

    if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0)
      endY = e.getY();
    if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0)
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
   * from the XORPainter interface. The actual drawing function.
   */
  public void XORDraw(int dx, int dy) 
  {

    Graphics g = gc.getGraphicDestination().getGraphics();
    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.

    if (ExplodeSelection.getSelection().size() > 20) {
      // if there are more then 20 elements to move, move the enclosing rect
    if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0) 
      enclosingRect.x += dx-previousX;

    if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0) 
      enclosingRect.y += dy-previousY;

    g.drawRect(enclosingRect.x,enclosingRect.y, enclosingRect.width, enclosingRect.height);
    }
    else // move every element
      {
	ScrEvent aScrEvent;
	for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
	  {
	    aScrEvent = (ScrEvent) e.nextElement();
	    
	    gc.getAdapter().setX(tempEvent, gc.getAdapter().getX(aScrEvent));
	    gc.getAdapter().setY(tempEvent, gc.getAdapter().getY(aScrEvent));
	    gc.getAdapter().setLenght(tempEvent, gc.getAdapter().getLenght(aScrEvent));
	    if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0) 
	      gc.getAdapter().setX(tempEvent, gc.getAdapter().getX(aScrEvent)+dx);
	    
	    if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0) 
	      gc.getAdapter().setY(tempEvent, gc.getAdapter().getY(aScrEvent)+dy);
	    
	    gc.getRenderer().getEventRenderer().render(tempEvent, g, true);
	  }
      }

    g.setPaintMode();
    g.setColor(Color.black);

    previousX = dx;
    previousY = dy;
    g.dispose();
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
