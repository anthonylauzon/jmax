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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;

import ircam.jmax.toolkit.*;

import java.awt.*;

/**
 * An IntegerValue event renderer. It is represented as a vertical black bar
 */
public class IntegerEventRenderer implements ObjectRenderer {

  public IntegerEventRenderer()
    {
    } 

  /**
   * constructor.
   */
  public IntegerEventRenderer(SequenceGraphicContext theGc) 
  {
    gc = theGc;
  }

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected)
    {
	render(obj, g, selected, gc);
    } 

  /**
   * draw the given event in the given graphic context.
   * It takes into account the selection state.
   */
  public void render(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
  {
      Event e = (Event) obj;
      MonoDimensionalAdapter adapter = (MonoDimensionalAdapter)((SequenceGraphicContext) theGc).getAdapter();
      TrackDataModel model = ((SequenceGraphicContext) theGc).getDataModel();

      int x = adapter.getX(e);
      int y = adapter.getY(e);    

      if (selected) 
	  g.setColor(Color.red);
      else 
	  g.setColor(Color.black);

      if(adapter.getViewMode() == MonoTrackEditor.BREAK_POINTS_VIEW)
	  {
	      g.fillOval(x-2, y-2, 5, 5);
	      if(selected)
		  g.drawOval(x-4, y-4, 9, 9);
	
	      Event next = model.getNextEvent(e);		

	      Event prev;
	      if(e instanceof UtilTrackEvent)//during the XOR draw
		  {
		      TrackEvent original = ((UtilTrackEvent)e).getOriginal();
		      prev = model.getPreviousEvent(e.getTime());
		      
		      if(prev == original)
			  prev = model.getPreviousEvent(original.getTime());
	      
		      if((prev != null)&&(!SequenceSelection.getCurrent().isInSelection(prev)))
			  g.drawLine(adapter.getX(prev), adapter.getY(prev), x, y);

		      if(next == original)
			  next = model.getNextEvent(original);
		  
		      if(next != null)
			 if (!SequenceSelection.getCurrent().isInSelection(next))
			     g.drawLine(x, y, adapter.getX(next), adapter.getY(next));
			 else
			     {//qui anche il next e' selezionato
				 g.drawLine(x, y, adapter.getX(next)+((UtilTrackEvent)e).getDeltaX(adapter), 
					    adapter.getY(next)+((UtilTrackEvent)e).getDeltaY(adapter));
			     }
		  }
	      else //normal paint
		  {
		      if(selected)
			  g.setColor(Color.black);

		      if(next != null)
			  g.drawLine(x, y, adapter.getX(next), adapter.getY(next)); 
		  }
	  }
      else
	  {
	      int heigth = adapter.getHeigth(e);    
	      int lenght = adapter.getLenght(e);

	      if(heigth<0)
		  {
		      y += heigth;
		      heigth = -heigth;
		  }

	      g.fillRect(x, y, lenght, heigth);
	  }
  }
  
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y) 
    {
	return contains(obj, x, y, gc);
    }

  /**
   * returns true if the given event contains the given (graphic) point
   */
  public boolean contains(Object obj, int x, int y, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    MonoDimensionalAdapter adapter = (MonoDimensionalAdapter)((SequenceGraphicContext) theGc).getAdapter();

    int evtx = adapter.getX(e);
    int evty = adapter.getY(e);    

    /* for now the height can be negative for the negative values (to speed the search)*/
    if(adapter.getViewMode() == MonoTrackEditor.BREAK_POINTS_VIEW)
	{
	    return ((evtx-POINT_RADIUS <= x) && (evtx+POINT_RADIUS >= x) &&
		    (evty-POINT_RADIUS <= y ) && (evty+POINT_RADIUS >= y));
	}
    else
	{
	    int evtheigth = adapter.getHeigth(e);
	    int evtlenght = adapter.getLenght(e);
	    if(evtheigth<0)
		{
		    evty += evtheigth;
		    evtheigth = -evtheigth;
		}

	    return (evtx<=x && (evtx+evtlenght >= x) && evty<=y && (evty+evtheigth) >= y);
	}
  }

  Rectangle eventRect = new Rectangle();
  Rectangle tempRect = new Rectangle();

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h)
    {
	return touches(obj, x, y, w, h, gc);
    } 

  /**
   * returns true if the representation of the given event "touches" the given rectangle
   */
  public boolean touches(Object obj, int x, int y, int w, int h, GraphicContext theGc) 
  {
    Event e = (Event) obj;
    MonoDimensionalAdapter adapter = (MonoDimensionalAdapter)((SequenceGraphicContext) theGc).getAdapter();

    int evtx = adapter.getX(e);
    int evty = adapter.getY(e);    
    
    if(adapter.getViewMode() == MonoTrackEditor.BREAK_POINTS_VIEW)
	{
	    eventRect.setBounds(evtx-POINT_RADIUS, evty-POINT_RADIUS, 2*POINT_RADIUS+1, 2*POINT_RADIUS+1);
	}
    else
	{
	    int evtheigth = adapter.getHeigth(e);
	    int evtlenght = adapter.getLenght(e);

	    if(evtheigth<0)
		{
		    evty += evtheigth;
		    evtheigth = -evtheigth;
		}

	    eventRect.setBounds(evtx, evty, evtlenght, evtheigth);	    
	}

    tempRect.setBounds(x, y, w, h);

    return  eventRect.intersects(tempRect);

  }

    public static IntegerEventRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new IntegerEventRenderer();

	return staticInstance;
    }

  //------------Fields
    final static int Integer_HEIGHT = 12;

    SequenceGraphicContext gc;
    public static IntegerEventRenderer staticInstance;
    public final static int INTEGER_WIDTH = 2;
    public final static int POINT_RADIUS = /*4*/5;
}
