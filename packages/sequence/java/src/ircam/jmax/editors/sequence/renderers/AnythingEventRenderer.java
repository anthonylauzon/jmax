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
public class AnythingEventRenderer implements SeqObjectRenderer {

  public AnythingEventRenderer()
    {
    } 

  /**
   * constructor.
   */
  public AnythingEventRenderer(SequenceGraphicContext theGc) 
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
      if(((Event)obj).isHighlighted())
	  render(obj, g, Event.HIGHLIGHTED, theGc); 
      else
	  if(selected)
	      render(obj, g, Event.SELECTED, theGc); 
	  else
	      render(obj, g, Event.DESELECTED, theGc); 
  }
  
  public void render(Object obj, Graphics g, int state, GraphicContext theGc) 
  {
      Event e = (Event) obj;
      MonoDimensionalAdapter adapter = (MonoDimensionalAdapter)((SequenceGraphicContext) theGc).getAdapter();
      TrackDataModel model = ((SequenceGraphicContext) theGc).getDataModel();

      int x = adapter.getX(e);
      int y = adapter.getY(e);;    

      switch(state)
	  {
	  case Event.SELECTED:
	      g.setColor(Color.red);
	      break;
	  case Event.DESELECTED:
	      g.setColor(Color.black);
	      break;
	  case Event.HIGHLIGHTED:
	      g.setColor(Color.green);
	      break;
	  }
      
      int heigth = 100;
      int lenght = 5;

      g.fillRect(x, y, lenght, heigth);
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

    int evtheigth = 100;
    int evtlenght = 5;
    if(evtheigth<0)
	{
	    evty += evtheigth;
	    evtheigth = -evtheigth;
	}
    
    return (evtx<=x && (evtx+evtlenght >= x) && evty<=y && (evty+evtheigth) >= y);
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
    
    int evtheigth = 100;
    int evtlenght = 5;

    eventRect.setBounds(evtx, evty, evtlenght, evtheigth);	    

    tempRect.setBounds(x, y, w, h);

    return  eventRect.intersects(tempRect);

  }

    public static AnythingEventRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new AnythingEventRenderer();

	return staticInstance;
    }

    SequenceGraphicContext gc;
    public static AnythingEventRenderer staticInstance;
}

