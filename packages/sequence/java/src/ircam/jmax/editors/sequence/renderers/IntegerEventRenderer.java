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
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int x = gc.getAdapter().getX(e);
    int y = gc.getAdapter().getY(e);    
    int heigth = gc.getAdapter().getHeigth(e);    
    int lenght = gc.getAdapter().getLenght(e); //fixed length

    //negative value
    /*if(((Integer)e.getProperty("integer")).intValue() < 0) 
      y -= heigth;*/

    /* for now the height can eb negatiev for the negative values (to speed the render)*/
    if(heigth<0)
	{
	    y += heigth;
	    heigth -= heigth;
	}

    if (selected) 
	g.setColor(Color.red);
    else 
	g.setColor(Color.black);

    //event's Rectangle
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
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);    
    int evtheigth = gc.getAdapter().getHeigth(e);
    int evtlenght = gc.getAdapter().getLenght(e);

    //negative value
    /*if(((Integer)e.getProperty("integer")).intValue() < 0) 
      evty -= evtheigth;*/

    /* for now the height can eb negatiev for the negative values (to speed the search)*/
    if(evtheigth<0)
	{
	    evty += evtheigth;
	    evtheigth -= evtheigth;
	}

    return  (evtx<=x && (evtx+evtlenght >= x) && evty<=y && (evty+evtheigth) >= y);
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
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e);
    int evty = gc.getAdapter().getY(e);    
    int evtheigth = gc.getAdapter().getHeigth(e);
    int evtlenght = gc.getAdapter().getLenght(e);

    //negative value
    /*if(((Integer)e.getProperty("integer")).intValue() < 0) 
      evty -= evtheigth;*/

    /* for now the height can eb negatiev for the negative values (to speed the search)*/
    if(evtheigth<0)
	{
	    evty += evtheigth;
	    evtheigth -= evtheigth;
	}

    eventRect.setBounds(evtx, evty, evtlenght, evtheigth);
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
}
