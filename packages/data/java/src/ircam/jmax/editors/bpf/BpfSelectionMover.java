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

package ircam.jmax.editors.bpf;

import ircam.jmax.editors.bpf.renderers.*;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * an interaction module used to move a selection of objects.
 * At the end of a move operation, it communicates the new position to the listener
 */
public class BpfSelectionMover extends SelectionMover  implements XORPainter {

    public BpfSelectionMover(DragListener theListener, int theMovement) 
    {
	super(theListener, theMovement); 
	
	initAutoScroll();
    }


    /******************* autoscrolling *******************/

    javax.swing.Timer scrollTimer;
    ScrollDragAction scroller;

    private void initAutoScroll()
    {
	scroller    = new ScrollDragAction();
	scrollTimer = new javax.swing.Timer(8, scroller);
	scrollTimer.setCoalesce(true);
	scrollTimer.setRepeats(true);
    }

    class ScrollDragAction implements ActionListener
    {
	BpfPanel bpfPanel;
	int x, y, delta;
	public void actionPerformed(ActionEvent ae)
	{
	    delta = bpfPanel.scrollBy(x, y);
	    updateStart(-delta, 0);
	    getListener().updateStartingPoint(-delta, 0);

	    BpfAdapter a = getGc().getAdapter();
	    getGc().getStatusBar().post(getGc().getToolManager().getCurrentTool(),
					" time "+a.getInvX(x));
	}
	void setEditor(BpfPanel editor)
	{
	    this.bpfPanel = editor;
	}
	void setXY(int x, int y)
	{
	    this.x = x;
	    this.y = y;
	}
    }

    void autoScrollIfNeeded(int x, int y)
    {
	BpfPanel panel = (BpfPanel)((Bpf)gc.getFrame()).getEditor();
	if (! panel.pointIsVisible(x , y))
	{
	    scroller.setXY(x, y);
	    if (!scrollTimer.isRunning())
		{
		    scroller.setEditor(panel);
		    scrollTimer.start();
		}
	}
	else 
	    {
		if (scrollTimer.isRunning())
		    {
			scrollTimer.stop();
		    }
	    }
    }

  /**
   * sets the point on which to start the movement
   */
  public void interactionBeginAt(int x, int y) 
  {
    super.interactionBeginAt(x, y);

    if (gc == null)
      System.err.println("-------------- GC NULL");

    if (BpfSelection.getCurrent().size() > 20) 
	{
	    dragMode = RECT_DRAG;
	    previousX=0;previousY=0;
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
    BpfPoint min_x;
    BpfPoint min_y;
    BpfPoint max_x;
    BpfPoint max_y;
    
    BpfPoint point;
    
    Enumeration e=((BpfGraphicContext)gc).getSelection().getSelected();
    point = (BpfPoint) e.nextElement();    
    
    if (point == null) return; //empty selection...
    else 
      {
	min_x = point;
	max_x = point;
	min_y = point;
	max_y = point;
      }

    BpfAdapter a = ((BpfGraphicContext) gc).getAdapter();
    
    for (; e.hasMoreElements();)
      {
	point = (BpfPoint) e.nextElement();

	if (a.getX(point) < a.getX(min_x)) 
	 min_x = point;
     
	if (a.getY(point) < a.getY(min_y)) 
	  min_y = point;
	
	if (a.getX(point) > a.getX(max_x)) 
	  max_x = point;
	
	if (a.getY(point) > a.getY(max_y)) 
	  max_y = point;
      }

    destination.setBounds(a.getX(min_x), a.getY(min_y),
			  a.getX(max_x)+a.getLenght(max_x)-a.getX(min_x),
			  a.getY(max_y)-a.getY(min_y)+10);
  }

    public void mouseReleased(MouseEvent e)
    {
	if (scrollTimer.isRunning())
	    scrollTimer.stop();
	super.mouseReleased(e);
    }

  /**
   * overrides SelectionMover.mouseDragged()
   */
  public void mouseDragged(MouseEvent e) 
  {
      if(!scrollTimer.isRunning())
	  super.mouseDragged(e);
      
      autoScrollIfNeeded(e.getX(), e.getY());
  }

  /**
   * from the XORPainter interface. The actual drawing function.
   */


  public void XORDraw(int dx, int dy) 
  {
    BpfPoint movPoint;
    Graphics g = gc.getGraphicDestination().getGraphics();
    
    Rectangle tempr, clip; 
    tempr = (Rectangle) g.getClip();
    clip = ((BpfGraphicContext)gc).getEditorClip();

    g.clipRect(clip.x, clip.y, clip.width, clip.height);
    
    g.setColor(Color.gray);

    if (dragMode == RECT_DRAG) {

      g.setXORMode(Color.white);

      if ((itsMovements & HORIZONTAL_MOVEMENT) != 0) 
	  enclosingRect.x += dx-previousX;
      
      if ((itsMovements & VERTICAL_MOVEMENT) != 0) 
	enclosingRect.y += dy-previousY;
      
      g.drawRect(enclosingRect.x, enclosingRect.y, enclosingRect.width, enclosingRect.height);
    }
    else // move every element
      {
	g.setXORMode(Color.gray); 

	BpfAdapter a = ((BpfGraphicContext) gc).getAdapter();
	boolean singleObject = BpfSelection.getCurrent().size()==1;
	
	BpfPoint last = BpfSelection.getCurrent().getLastSelectedPoint();

	for (Enumeration e = BpfSelection.getCurrent().getSelected(); e.hasMoreElements();)
	  {
	    movPoint = (BpfPoint) e.nextElement();

	    tempPoint.setOriginal(movPoint);

	    tempPoint.setTime(movPoint.getTime());
	    
	    tempPoint.setValue(movPoint.getValue());

	    if ((itsMovements & HORIZONTAL_MOVEMENT) != 0)
		{ 
		    int prevX = 0;
		    int nextX = 0;
		    FtsBpfObject ftsObj = ((BpfGraphicContext)gc).getFtsObject();
		    BpfPoint next = ftsObj.getNextPoint(movPoint);
		    if(next!=null)
			nextX = a.getX(next)-1;
		    BpfPoint prev = ftsObj.getPreviousPoint(movPoint.getTime());
		    if(prev!=null)
			prevX = a.getX(prev)+1;
		    
		    if((a.getX(movPoint) + dx > nextX)&&(next!=null))
			tempPoint.setTime(a.getInvX(nextX));
		    else
			if((a.getX(movPoint) + dx < prevX)&&(prev!=null))
			    tempPoint.setTime(a.getInvX(prevX));
			else
			    tempPoint.setTime(a.getInvX(a.getX(movPoint) + dx));
		}

	    if ((itsMovements & VERTICAL_MOVEMENT) != 0) 
		tempPoint.setValue(a.getInvY(a.getY(movPoint)+dy));

	    tempPoint.setDeltaX(dx);

	    PointRenderer.getRenderer().render(tempPoint, g, true, gc);
	  }
      }
    
    g.setPaintMode();
    g.setColor(Color.black);
    
    previousX = dx;
    previousY = dy;
    
    g.setClip(tempr);
    g.dispose();
  }

    void updateStart(int deltaX, int deltaY)
    {
	itsStartingPoint.x+=deltaX;
	itsXORHandler.updateBegin(deltaX, deltaY);
    }

    DragListener getListener()
    {
	return itsListener;
    }

    BpfGraphicContext getGc()
    {
	return (BpfGraphicContext)gc;
    }

    int getMovements()
    {
	return itsMovements;
    }

  //--- Fields
    Rectangle enclosingRect = new Rectangle();
    UtilBpfPoint tempPoint = new UtilBpfPoint();
}


















