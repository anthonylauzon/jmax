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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;

/**
 * an interaction module used to move a selection of objects.
 * At the end of a move operation, it communicates the new position to the listener
 */
public class ExplodeSelectionMover extends SelectionMover  implements XORPainter {

  public ExplodeSelectionMover(DragListener theListener, int theMovement) 
  {
    super(theListener, theMovement); 
  }

  /**
   * sets the point on which to start the movement
   */
  public void interactionBeginAt(int x, int y) 
  {
    super.interactionBeginAt(x, y);

    if (gc == null)
      System.err.println("-------------- GC NULL");

    if (((ExplodeGraphicContext)gc).getSelection().size() > 20) 
      {
	dragMode = RECT_DRAG;
	computeEnclosure(enclosingRect);
      }
    else dragMode = GROUP_DRAG;

  }

  /**
   * computes the rectangle that surrounds the selection
   * and sets the given rectangle.
   */
  private void computeEnclosure(Rectangle destination) 
  {  
    ScrEvent min_x;
    ScrEvent min_y;
    ScrEvent max_x;
    ScrEvent max_y;
    
    ScrEvent aEvent;
    
    Enumeration e=((ExplodeGraphicContext)gc).getSelection().getSelected();
    aEvent = (ScrEvent) e.nextElement();    
    
    if (aEvent == null) return; //empty selection...
    else 
      {
	min_x = aEvent;
	max_x = aEvent;
	min_y = aEvent;
	max_y = aEvent;
      }

    Adapter a = ((ExplodeGraphicContext) gc).getAdapter();
    
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
   * overrides SelectionMover.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    int deltaX = egc.getAdapter().getInvX(e.getX()) - egc.getAdapter().getInvX(itsStartingPoint.x);
    int deltaY = egc.getAdapter().getInvY(e.getY()) - egc.getAdapter().getInvY(itsStartingPoint.y);
    
    if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0)
      egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), " dx "+(deltaX));
    else if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0)
      egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), " dy "+deltaY);
    else egc.getStatusBar().post(egc.getToolManager().getCurrentTool(), " dx "+(deltaX)+", dy "+(deltaY));

    super.mouseDragged(e);
  }

  /**
   * from the XORPainter interface. The actual drawing function.
   */
  public void XORDraw(int dx, int dy) 
  {

    Graphics g = gc.getGraphicDestination().getGraphics();
    g.setColor(Color.gray);
    g.setXORMode(Color.white); 

    if (dragMode == RECT_DRAG) {
      
      if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0) 
	enclosingRect.x += dx-previousX;
      
      if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0) 
	enclosingRect.y += dy-previousY;
      
      g.drawRect(enclosingRect.x,enclosingRect.y, enclosingRect.width, enclosingRect.height);
    }
    else // move every element
      {
	ScrEvent aScrEvent;
	Adapter a = ((ExplodeGraphicContext) gc).getAdapter();
	ObjectRenderer er = gc.getRenderManager().getObjectRenderer();
	boolean singleObject = ((ExplodeGraphicContext)gc).getSelection().size()==1;
	for (Enumeration e = ((ExplodeGraphicContext)gc).getSelection().getSelected(); e.hasMoreElements();)
	  {
	    aScrEvent = (ScrEvent) e.nextElement();

	    a.setX(tempEvent, a.getX(aScrEvent));
	    a.setY(tempEvent, a.getY(aScrEvent));
	    a.setLenght(tempEvent, a.getLenght(aScrEvent));
	    if ((itsMovements & MoverTool.HORIZONTAL_MOVEMENT) != 0) 
	      a.setX(tempEvent, a.getX(aScrEvent)+dx);
	    
	    if ((itsMovements & MoverTool.VERTICAL_MOVEMENT) != 0) 
	      a.setY(tempEvent, a.getY(aScrEvent)+dy);
	    
	    er.render(tempEvent, g, true, gc);
	    if (singleObject) 
	      ScoreBackground.pressKey(tempEvent.getPitch(), gc);
	  }
      }
    
    g.setPaintMode();
    g.setColor(Color.black);
    
    previousX = dx;
    previousY = dy;
    g.dispose();
  }

  //--- Fields
  Rectangle enclosingRect = new Rectangle();
  ScrEvent tempEvent = new ScrEvent(null);
}
