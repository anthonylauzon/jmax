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

package ircam.jmax.editors.sequence.renderers;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;
import ircam.jmax.editors.sequence.track.Event;
import ircam.jmax.toolkit.*;
import ircam.fts.client.*;

import java.awt.*;

/**
* The piano-roll event renderer in a Score with an ambitus: the line-based event, 
 * with a length , variable width, black color, a label.
 */
public class MarkerEventRenderer implements SeqObjectRenderer {
	
  public MarkerEventRenderer()
{
} 

/**
* constructor.
 */
public MarkerEventRenderer(SequenceGraphicContext theGc) 
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
	SequenceGraphicContext gc = (SequenceGraphicContext) theGc;
	Dimension d = gc.getGraphicDestination().getSize();	
	PartitionAdapter pa = (PartitionAdapter)(gc.getAdapter());
	
	int maxY;
	if( gc.isInSequence())
		maxY = d.height;
	else
		maxY = pa.getMaxScoreY();
	
	int minY = pa.getMinScoreY();
	String type = (String)(e.getProperty("type"));
	
	int x = pa.getX(e);
	Color col, lightCol;
  
  if( e instanceof UtilTrackEvent)
  {
    col = Color.red;
    lightCol = Color.red;
  }
  else
  {
    switch(state)
		{
      case Event.SELECTED:
        col = selMarkColor;
        lightCol = lightSelMarkColor;
        break;
      case Event.HIGHLIGHTED:
        col = highMarkColor;
        lightCol = lightHighColor;
        break;
      default:
      case Event.DESELECTED:
        col = markColor;
        lightCol = lightMarkColor;
        break;
		}
  }
  if(type.equals("marker"))
  {
    g.setColor( lightCol);
    g.drawLine( x, maxY, x, 0);
  }
  else
  {
    g.setColor( col);
    g.drawLine( x, minY, x, maxY);
    g.setColor( lightCol);
    g.drawLine( x, maxY, x, 0);
  }
}

public void renderBounds(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
{
	render(obj, g, selected, theGc);
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
	return (x <= evtx+2 && x >= evtx-2);		
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
	tempRect.setBounds(x, y, w, h);
	eventRect.setBounds(evtx-2, 0, 5, gc.getGraphicDestination().getSize().height);
	return  tempRect.intersects(eventRect);
}

public static MarkerEventRenderer getRenderer()
{
	if (staticInstance == null)
		staticInstance = new MarkerEventRenderer();
	
	return staticInstance;
}

//------------Fields
final static int NOTE_DEFAULT_WIDTH = 5;
public SequenceGraphicContext gc;
public static MarkerEventRenderer staticInstance;

Color lightMarkColor = new Color(165, 165, 165, 100);
Color lightSelMarkColor = new Color(255, 0, 0, 100);
Color lightHighColor = new Color(0, 255, 0, 100);

Color markColor = new Color( 0, 0, 0, 180);
Color selMarkColor = new Color(255, 0, 0, 180);
Color highMarkColor = new Color(0, 255, 0, 180);

int oldX, oldY;
}

