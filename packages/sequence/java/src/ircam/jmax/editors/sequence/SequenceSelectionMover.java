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

package ircam.jmax.editors.sequence;

import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.editors.sequence.renderers.*;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * an interaction module used to move a selection of objects.
 * At the end of a move operation, it communicates the new position to the listener
 */
public class SequenceSelectionMover extends SelectionMover  implements XORPainter {

    public SequenceSelectionMover(DragListener theListener, int theMovement) 
    {
	super(theListener, theMovement); 
	
	initAutoScroll();
    }


    /******************* autoscrolling *******************/

    Timer scrollTimer;
    SequenceScrollDragAction scroller;

    private void initAutoScroll()
    {
	scroller    = new SequenceScrollDragAction();
	scrollTimer = new Timer(8, scroller);
	scrollTimer.setCoalesce(true);
	scrollTimer.setRepeats(true);
    }

    class SequenceScrollDragAction implements ActionListener
    {
	SequencePanel sequencePanel;
	int x, y, delta;
	public void actionPerformed(ActionEvent ae)
	{
	    delta = sequencePanel.scrollBy(x, y);
	    updateStart(-delta, 0);
	    getListener().updateStartingPoint(-delta, 0);

	    PartitionAdapter pa = ((PartitionAdapter)getGc().getAdapter());
	    getGc().getStatusBar().post(getGc().getToolManager().getCurrentTool(), " time "+pa.getInvX(x));
	}
	void setEditor(SequencePanel editor)
	{
	    this.sequencePanel = editor;
	}
	void setXY(int x, int y)
	{
	    this.x = x;
	    this.y = y;
	}
    }

    void autoScrollIfNeeded(int x, int y)
    {
	SequencePanel panel = (SequencePanel)((Sequence)gc.getFrame()).getEditor();
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

    if (((SequenceGraphicContext)gc).getSelection().size() > 20) 
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
    TrackEvent min_x;
    TrackEvent min_y;
    TrackEvent max_x;
    TrackEvent max_y;
    
    TrackEvent aEvent;
    
    Enumeration e=((SequenceGraphicContext)gc).getSelection().getSelected();
    aEvent = (TrackEvent) e.nextElement();    
    
    if (aEvent == null) return; //empty selection...
    else 
      {
	min_x = aEvent;
	max_x = aEvent;
	min_y = aEvent;
	max_y = aEvent;
      }

    Adapter a = ((SequenceGraphicContext) gc).getAdapter();
    
    for (; e.hasMoreElements();)
      {
	aEvent = (TrackEvent) e.nextElement();

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
      SequenceGraphicContext egc = (SequenceGraphicContext) gc;
      
      if(!scrollTimer.isRunning())
	  super.mouseDragged(e);
      
      autoScrollIfNeeded(e.getX(), e.getY());
  }

  /**
   * from the XORPainter interface. The actual drawing function.
   */


  public void XORDraw(int dx, int dy) 
  {
    TrackEvent movTrackEvent;
    Graphics g = gc.getGraphicDestination().getGraphics();
    
    Rectangle tempr, clip; 
    tempr = (Rectangle) g.getClip();
    clip = ((SequenceGraphicContext)gc).getTrackClip();

    g.clipRect(clip.x, clip.y, clip.width, clip.height);
    
    g.setColor(Color.gray);
    g.setXORMode(Color.white); 

    if (dragMode == RECT_DRAG) {
      
      if ((itsMovements & HORIZONTAL_MOVEMENT) != 0) 
	enclosingRect.x += dx-previousX;
      
      if ((itsMovements & VERTICAL_MOVEMENT) != 0) 
	enclosingRect.y += dy-previousY;
      
      g.drawRect(enclosingRect.x, enclosingRect.y, enclosingRect.width, enclosingRect.height);
    }
    else // move every element
      {
	PartitionAdapter a = (PartitionAdapter)((SequenceGraphicContext) gc).getAdapter();
	boolean singleObject = ((SequenceGraphicContext)gc).getSelection().size()==1;
	
	TrackEvent last = ((SequenceGraphicContext)gc).getSelection().getLastSelectedEvent();

	for (Enumeration e = ((SequenceGraphicContext)gc).getSelection().getSelected(); e.hasMoreElements();)
	  {
	    movTrackEvent = (TrackEvent) e.nextElement();

	    tempEvent.setOriginal(movTrackEvent);

	    //a.setX(tempEvent, a.getX(movTrackEvent));
	    tempEvent.setTime(movTrackEvent.getTime());
	    
	    a.setY(tempEvent, a.getY(movTrackEvent));
	    a.setLabel(tempEvent, a.getLabel(movTrackEvent));//???

	    a.setLenght(tempEvent, a.getLenght(movTrackEvent));
	    a.setHeigth(tempEvent, a.getHeigth(movTrackEvent));
	    if ((itsMovements & HORIZONTAL_MOVEMENT) != 0) 
		a.setX(tempEvent, a.getX(movTrackEvent) + dx);

	    if ((itsMovements & VERTICAL_MOVEMENT) != 0) 
		    a.setY(tempEvent, a.getY(movTrackEvent)+dy);

	    movTrackEvent.getRenderer().render(tempEvent, g, true, gc);

	    // e_m_ incorrect! instead, make this object communicate the new position to the listeners,
	    // and make the keyboard in the MidiTrack a listener of such movements.
	    // (something like using the ircam.jmax.toolkit.DynamicDragListener).
	    if ((singleObject)&&(a.getViewMode()==MidiTrackEditor.PIANOROLL_VIEW)) 
	      ScoreBackground.pressKey(((Integer)tempEvent.getProperty("pitch")).intValue(), getGc());
	  
	    if(movTrackEvent == last) 
		{
		    if ((itsMovements & HORIZONTAL_MOVEMENT) != 0)
			((SequenceGraphicContext)gc).getStatusBar().
			    post(((SequenceGraphicContext)gc).
				 getToolManager().getCurrentTool(), a.XMapper.getName()+" "+tempEvent.getTime());
		    if ((itsMovements & VERTICAL_MOVEMENT) != 0)
			((SequenceGraphicContext)gc).getStatusBar().
			    post(((SequenceGraphicContext)gc).
				 getToolManager().getCurrentTool(), a.YMapper.getName()+" "+a.getInvY(a.getY(tempEvent)));
		}
	  }
      }
    
    g.setPaintMode();
    g.setColor(Color.black);
    
    previousX = dx;
    previousY = dy;
    
    g.setClip(tempr);//????
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

    SequenceGraphicContext getGc()
    {
	return (SequenceGraphicContext)gc;
    }

    int getMovements()
    {
	return itsMovements;
    }

  //--- Fields
    Rectangle enclosingRect = new Rectangle();
    UtilTrackEvent tempEvent = new UtilTrackEvent(new AmbitusValue());
    // every event type would be OK, but we also need to handle the little keyboard in the
    // left side of the window... so we need an event that knows about the "pitch" property
}


















