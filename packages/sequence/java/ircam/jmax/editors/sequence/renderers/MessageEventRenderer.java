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
 * The renderer of a string in a track: simply writes the string in the position given by
 * the adapter.
 */
public class MessageEventRenderer implements SeqObjectRenderer {

    public MessageEventRenderer()
    {
    } 

  /**
   * constructor.
   */
    public MessageEventRenderer(SequenceGraphicContext theGc) 
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
  
    public void render(Object obj, Graphics g, int state, GraphicContext theGc) 
    {
	Event e = (Event) obj;

	SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(stringFont);
	String mess = gc.getAdapter().getLabel(e);
	boolean open = ((Boolean)e.getProperty("open")).booleanValue();

	int x = gc.getAdapter().getX(e);
	int y = gc.getAdapter().getY(e);

	int width = ((MessageAdapter)gc.getAdapter()).getLenght(e)-BUTTON_WIDTH;	
	
	int height = ((Integer)e.getProperty("height")).intValue()+2;
	if(height==0)
	    height = fm.getHeight();

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

	g.fillRect(x, y, BUTTON_WIDTH, height);

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
	
	g.drawRect(x, y, BUTTON_WIDTH, height);

	if(open)
	    {
		backupFont = g.getFont();

		g.setColor(Color.white);
		g.fillRect(x+BUTTON_WIDTH+1, y, width, height);

		switch(state)
		    {
		    case Event.SELECTED:
			g.setColor(Color.red);
			break;
		    case Event.DESELECTED:
			g.setColor(Color.darkGray);
			break;
		    case Event.HIGHLIGHTED:
			g.setColor(Color.green);
		    }

		g.drawRect(x+BUTTON_WIDTH+1, y, width, height);

		if(e instanceof TrackEvent)
		    TextRenderer.getRenderer().render(g, mess, x+BUTTON_WIDTH+1+2, y+1, width-2, height-2);
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
	SequenceGraphicContext gc = (SequenceGraphicContext) theGc;
	MessageAdapter a = (MessageAdapter)gc.getAdapter();
    
	int evtx = a.getX(e);
	int evty = a.getY(e);

	int evtLenght = a.getLenght(e);
	int evtHeight = ((Integer)e.getProperty("height")).intValue()+2;
	
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
	MessageAdapter a = (MessageAdapter)gc.getAdapter();

	int evtx = a.getX(e);
	int evty = a.getY(e);
	
	int evtLenght = a.getLenght(e);
	int evtHeight = ((Integer)e.getProperty("height")).intValue()+2;

	eventRect.setBounds(evtx, evty, evtLenght, evtHeight);
	tempRect.setBounds(x, y, w, h);
	return  eventRect.intersects(tempRect);
    }

    public static MessageEventRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new MessageEventRenderer();

	return staticInstance;
    }

    //------------Fields

    SequenceGraphicContext gc;
    public static MessageEventRenderer staticInstance;

    public final static int DEFAULT_TEXT_WIDTH = 48; 
    public final static int MINIMUM_WIDTH = 30;
    public static int BUTTON_WIDTH = 10; 
    public final static int DEFAULT_WIDTH = DEFAULT_TEXT_WIDTH+BUTTON_WIDTH;

    //public static Font stringFont = new Font("SansSerif", Font.BOLD, 12); 
    public static Font stringFont = new Font("SansSerif", Font.PLAIN, 10); 
    private Font backupFont;
}



