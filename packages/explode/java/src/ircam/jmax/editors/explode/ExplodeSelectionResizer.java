package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * an interaction module used to resize a selection of explode objects.
 * At the end of a resize operation, it communicates the new position to the listener
 */
public class ExplodeSelectionResizer extends SelectionResizer {

  /**
   * constructor. 
   */
  public ExplodeSelectionResizer(DragListener theListener) 
  {
    super(theListener);
    
  }
  
  /**
   * overrides SelectionResizer.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    int deltaX = egc.getAdapter().getInvX(e.getX()) - egc.getAdapter().getInvX(itsStartingPoint.x);
    int deltaY = egc.getAdapter().getInvY(e.getY()) - egc.getAdapter().getInvY(itsStartingPoint.y);
    
    egc.getStatusBar().post(Explode.toolbar.getTool(), " dx "+deltaX);

    super.mouseDragged(e);
  }



  /**
   * from the XORPainter interface. The actual drawing function.
   */
  public void XORDraw(int dx, int dy) 
  {
    previousX = dx;
    previousY = dy;

    Graphics g = gc.getGraphicDestination().getGraphics();
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    g.setColor(Color.darkGray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.
    
    Adapter a = egc.getAdapter();
    ObjectRenderer er = egc.getRenderManager().getObjectRenderer();
    
    ScrEvent aScrEvent;
    for (Enumeration e = ExplodeSelection.getSelection().getSelected(); e.hasMoreElements();)
      {
	aScrEvent = (ScrEvent) e.nextElement();
	a.setX(tempEvent, a.getX(aScrEvent));
	a.setY(tempEvent, a.getY(aScrEvent));

	a.setLenght(tempEvent, a.getLenght(aScrEvent)+dx);
	er.render(tempEvent, g, false);
      }
	 
    g.setPaintMode();
    g.setColor(Color.black);
    g.dispose();
  }

  //---- Fields

  ScrEvent tempEvent = new ScrEvent(null);
}
