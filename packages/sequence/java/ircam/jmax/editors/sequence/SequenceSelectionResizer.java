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
import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * an interaction module used to resize a selection of sequence objects.
 * At the end of a resize operation, it communicates the new position to the listener
 */
public class SequenceSelectionResizer extends SelectionResizer {

  /**
   * constructor. 
   */
  public SequenceSelectionResizer(DragListener theListener) 
  {
    super(theListener);

    initAutoScroll();
  }
  
  /******************* autoscrolling *******************/

  javax.swing.Timer scrollTimer;
  SequenceScrollDragAction scroller;

  private void initAutoScroll()
  {
    scroller    = new SequenceScrollDragAction();
    scrollTimer = new javax.swing.Timer(8, scroller);
    scrollTimer.setCoalesce(true);
    scrollTimer.setRepeats(true);
  }

  class SequenceScrollDragAction implements ActionListener
  {
    ScrollManager scrollManager;
    int x, y, delta;
    public void actionPerformed(ActionEvent ae)
    {
      delta = scrollManager.scrollBy(x, y);
      updateStart(-delta, 0);
      getListener().updateStartingPoint(-delta, 0);
	    
      PartitionAdapter pa = ((PartitionAdapter)getGc().getAdapter());
      getGc().getStatusBar().post( getGc().getToolManager().getCurrentTool(), 
				   pa.LenghtMapper.getName()+" "+pa.getInvX(x));
    }
    void setScrollManager( ScrollManager mng)
    {
      this.scrollManager = mng;
    }
    void setXY(int x, int y)
    {
      this.x = x;
      this.y = y;
    }
  }

  void autoScrollIfNeeded(int x, int y)
  {
    ScrollManager manager = getGc().getScrollManager();
    if (! manager.pointIsVisible(x , y))
      {
	scroller.setXY(x, y);
	if (!scrollTimer.isRunning())
	  {
	    scroller.setScrollManager( manager);
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

  public void mouseReleased( MouseEvent e)
  {
    if ( scrollTimer.isRunning())
      scrollTimer.stop();
    super.mouseReleased( e);
  }

  public void mouseDragged( MouseEvent e) 
  { 
    autoScrollIfNeeded( e.getX(), e.getY());
    if( !scrollTimer.isRunning())
      super.mouseDragged( e);
  }
  
  void updateStart( int deltaX, int deltaY)
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

  /**
   * from the XORPainter interface. The actual drawing function.
   */
  public void XORDraw(int dx, int dy) 
  {
    previousX = dx;
    previousY = dy;

    Graphics g = gc.getGraphicDestination().getGraphics();
    SequenceGraphicContext egc = (SequenceGraphicContext) gc;
    PartitionAdapter a = (PartitionAdapter)egc.getAdapter();

    Rectangle tempr, clip; 
    tempr = (Rectangle) g.getClip();
    clip = ((SequenceGraphicContext)gc).getTrackClip();

    g.clipRect(clip.x, clip.y, clip.width, clip.height);

    g.setColor(Color.gray);
    g.setXORMode(Color.white); //there's an assumption here on the color of the background.
    
    TrackEvent last = egc.getSelection().getLastSelectedEvent();

    TrackEvent aTrackEvent;
    for (Enumeration e = egc.getSelection().getSelected(); e.hasMoreElements();)
      {
	aTrackEvent = (TrackEvent) e.nextElement();
	a.setX(tempEvent, a.getX(aTrackEvent));
	a.setY(tempEvent, a.getY(aTrackEvent));

	a.setLenght(tempEvent, a.getLenght(aTrackEvent)+dx);
	aTrackEvent.getRenderer().render(tempEvent, g, false, egc);
	
	if(aTrackEvent == last) 
	    egc.getStatusBar().post(egc.getToolManager().getCurrentTool(),
				    a.LenghtMapper.getName()+" "+a.getInvLenght(tempEvent));
      }
	 
    g.setPaintMode();
    g.setColor(Color.black);

    g.setClip(tempr);//????
    g.dispose();
  }

  //---- Fields

  UtilTrackEvent tempEvent = new UtilTrackEvent(new AmbitusValue());
}







