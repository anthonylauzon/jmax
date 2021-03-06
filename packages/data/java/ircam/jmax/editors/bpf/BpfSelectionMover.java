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
	BpfPanel panel = (BpfPanel)((BpfWindow)gc.getFrame()).getEditor();
	
	if (! panel.pointIsVisible(x , y) && panel.pointIsScrollable(x, y))
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

    previousX=0;
    previousY=0;

    if (((BpfGraphicContext)gc).getSelection().size() > 20) 
	{
	    dragMode = RECT_DRAG;
	    //previousX=0;previousY=0;
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
    int min_x, min_y, max_x, max_y;
    
    BpfPoint point;
    BpfAdapter a = ((BpfGraphicContext) gc).getAdapter();
    
    Enumeration e=((BpfGraphicContext)gc).getSelection().getSelected();
    point = (BpfPoint) e.nextElement();    
    
    if (point == null) return; //empty selection...
    else 
      {
	  min_x = max_x = a.getX(point);
	  min_y = max_y = a.getY(point);
      }

    int x, y;
    for (; e.hasMoreElements();)
      {
	point = (BpfPoint) e.nextElement();
	x = a.getX(point);
	y = a.getY(point);
	
	if (x < min_x) min_x = x;
	else if (x > max_x) max_x = x;	
	
	if (y < min_y) min_y = y;
	else if (y > max_y) max_y = y;
      }

    destination.setBounds(min_x, min_y, max_x-min_x, max_y-min_y);
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
    BpfGraphicContext bgc = (BpfGraphicContext)gc;
    BpfAdapter a = bgc.getAdapter();
    FtsBpfObject ftsObj = bgc.getFtsObject();
    Rectangle tempr, clip; 
    tempr = (Rectangle) g.getClip();
    clip = bgc.getEditorClip();

    g.clipRect(clip.x, clip.y, clip.width, clip.height);
    
    //clip deltaY///////////////////
    if(dy > 0)
    {
	int minY = a.getY(bgc.getSelection().getMinValueInSelection());
	int hMin = a.getY(ftsObj.getMinimumValue());
	if(minY + dy > hMin)
	  dy = hMin - minY;
    }
    else
    {
	int maxY = a.getY(bgc.getSelection().getMaxValueInSelection());
	int hMax = a.getY(ftsObj.getMaximumValue());
	if(maxY + dy < hMax)
	  dy = hMax - maxY;
    }
	
    //// Clip deltaX
    BpfPoint last, first;
    first =  bgc.getSelection().getFirstInSelection();
    if(bgc.getSelection().size()==1)
	last = first;
    else
	last =  bgc.getSelection().getLastInSelection();
    int lastX =  a.getX(last);
    int firstX = a.getX(first);

    BpfPoint next = ftsObj.getNextPoint(last);
    BpfPoint prev = ftsObj.getPreviousPoint(first);
    int nextX = -1;
    int prevX = -1;
    if(next!=null)
	nextX = a.getX(next);
    if(prev!=null)
      prevX = a.getX(prev);
    else
      prevX = a.getX(0);

    if((next != null)&&(lastX+dx > nextX)) dx = nextX-lastX;
    else if(firstX+dx < prevX) dx = prevX-firstX;

    ////////////////////////
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

	for (Enumeration e = bgc.getSelection().getSelected(); e.hasMoreElements();)
	  {
	    movPoint = (BpfPoint) e.nextElement();

	    tempPoint.setOriginal(movPoint);

	    tempPoint.setTime(movPoint.getTime());
	    
	    tempPoint.setValue(movPoint.getValue());

	    if ((itsMovements & HORIZONTAL_MOVEMENT) != 0)
		tempPoint.setTime(a.getInvX(a.getX(movPoint) + dx));		    

	    if ((itsMovements & VERTICAL_MOVEMENT) != 0) 
		tempPoint.setValue(a.getInvY(a.getY(movPoint)+dy));

	    tempPoint.setDeltaX(dx);

	    PointRenderer.getRenderer().render(tempPoint, g, true, bgc);
	  }
      }

    displayMovingSelectionInfo(dx, dy);
    
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

  void displayMovingSelectionInfo(int dx, int dy)
  {
    BpfGraphicContext bgc = (BpfGraphicContext)gc;
    BpfAdapter a = bgc.getAdapter();

    BpfPoint first = bgc.getSelection().getFirstInSelection();
    BpfPoint last = bgc.getSelection().getLastInSelection();
    BpfPoint prev = bgc.getFtsObject().getPreviousPoint(first);

    String text = "("+PointRenderer.numberFormat.format(a.getInvX(a.getX(first)+dx))+" , "+
      PointRenderer.numberFormat.format(a.getInvY(a.getY(first)+dy))+" )  ";
    
    if(bgc.getSelection().size() > 1)
      text = text+"["+(int)(last.getTime()-first.getTime())+"]";
    
    if(prev!=null)
      {
	float prevTime = a.getInvX(a.getX(first)+dx) - prev.getTime();
	if(prevTime < 0) prevTime = 0;
	text = PointRenderer.numberFormat.format(prevTime)+" --> "+text;
      }
    bgc.displayInfo(text);
  }

  //--- Fields
  Rectangle enclosingRect = new Rectangle();
  UtilBpfPoint tempPoint = new UtilBpfPoint();
}


















