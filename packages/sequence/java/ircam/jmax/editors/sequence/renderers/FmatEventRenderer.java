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

import java.awt.*;

/**
* The renderer of a string in a track: simply writes the string in the position given by
 * the adapter.
 */
public class FmatEventRenderer implements SeqObjectRenderer {
  
  public FmatEventRenderer()
{
} 

/**
* constructor.
 */
public FmatEventRenderer(SequenceGraphicContext theGc) 
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
* Draw the given event in the given graphic context.
 * It takes into account the selection state.
 */
public void render(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
{
	if(selected)
    render(obj, g, Event.SELECTED, theGc); 
	else
    render(obj, g, Event.DESELECTED, theGc); 
}
public void renderBounds(Object obj, Graphics g, boolean selected, GraphicContext theGc) 
{
  render( obj, g, selected, theGc);
}

public void render(Object obj, Graphics g, int state, GraphicContext theGc) 
{
  Event e = (Event) obj;
  
  SequenceGraphicContext gc = (SequenceGraphicContext) theGc;
	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(stringFont);
  Dimension d = gc.getGraphicDestination().getSize();
  
	int x = gc.getAdapter().getX(e);
  int y = (d.height - DEFAULT_HEIGHT)/2;
  
	int width = ((MonoDimensionalAdapter)gc.getAdapter()).getLenght(e);	
	int height = DEFAULT_HEIGHT+2;  
  
	switch(state)
  {
    case Event.SELECTED:
      g.setColor(Color.pink);
      break;
    case Event.DESELECTED:
      g.setColor(Color.lightGray);
      break;
    case Event.HIGHLIGHTED:
      g.setColor(Color.green.brighter());
  }
  
	g.fill3DRect(x+1, y+1, width-1, height-1, true);
    
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
  }
	
	g.drawRect(x, y, width, height);
  g.drawLine(x, 0, x, d.height);
      
		/*if(e instanceof TrackEvent)
		    TextRenderer.getRenderer().render(g, mess, x+BUTTON_WIDTH+1+2, y+1, width-2, height-2);*/
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
	MonoDimensionalAdapter a = (MonoDimensionalAdapter)gc.getAdapter();
  Dimension d = gc.getGraphicDestination().getSize();
  
	int evtx = a.getX(e);
	int evty = (d.height - DEFAULT_HEIGHT)/2;
  
	int evtLenght = a.getLenght(e);
	int evtHeight = DEFAULT_HEIGHT+2;
	
	tempRect.setBounds(evtx, evty, evtLenght, evtHeight);
  
	return  tempRect.contains(x, y);
}


/**
* returns true if the representation of the given event "touches" the given rectangle
 */
public boolean touches(Object obj, int x, int y, int w, int h)
{
	return touches(obj, x, y, w, h, gc);
} 

Rectangle eventRect = new Rectangle();
Rectangle tempRect = new Rectangle();

/**
* returns true if the representation of the given event "touches" the given rectangle
 */
public boolean touches(Object obj, int x, int y, int w, int h, GraphicContext theGc) 
{
  Event e = (Event) obj;
  SequenceGraphicContext gc = (SequenceGraphicContext) theGc;
  MonoDimensionalAdapter a = (MonoDimensionalAdapter)gc.getAdapter();
  Dimension d = gc.getGraphicDestination().getSize();
  
  int evtx = a.getX(e);
  int evty = (d.height - DEFAULT_HEIGHT)/2;
	
  int evtLenght = a.getLenght(e);
  int evtHeight = DEFAULT_HEIGHT+2;
  
  eventRect.setBounds(evtx, evty, evtLenght, evtHeight);
  tempRect.setBounds(x, y, w, h);
  return  eventRect.intersects(tempRect);
}

public static FmatEventRenderer getRenderer()
{
	if (staticInstance == null)
    staticInstance = new FmatEventRenderer();
  
	return staticInstance;
}

//------------Fields

SequenceGraphicContext gc;
public static FmatEventRenderer staticInstance;

public final static int MINIMUM_WIDTH = 20;
public final static int DEFAULT_WIDTH = 40;
public final static int DEFAULT_HEIGHT = 50;

public static Font stringFont = new Font("SansSerif", Font.PLAIN, 10); 
private Font backupFont;
}



