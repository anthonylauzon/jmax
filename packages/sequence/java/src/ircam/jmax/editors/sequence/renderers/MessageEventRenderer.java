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
public class MessageEventRenderer implements ObjectRenderer {

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
	Event e = (Event) obj;
	SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(stringFont);
	//String mess = (String) e.getProperty("message");
	String mess = gc.getAdapter().getLabel(e);

	int x = gc.getAdapter().getX(e);
	int y = gc.getAdapter().getY(e);
	int width = fm.stringWidth(mess)+4;
	int height = fm.getHeight();

	if (selected) 
	    g.setColor(Color.red);
	else 
	    g.setColor(Color.black);

	backupFont = g.getFont();

	g.drawRect(x, y, width, height);

	g.setFont(stringFont);
	//g.drawString((String) e.getProperty("message"), x+2, y+height-2);
	g.drawString(mess, x+2, y+height-2);

	g.setFont(backupFont);
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
	Adapter a = gc.getAdapter();
    
	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(stringFont);
	String mess = (String) e.getProperty("message");

	int evtx = a.getX(e);
	int evty = a.getY(e);
	int evtLenght = fm.stringWidth(mess)+4;
	int evtHeight = fm.getHeight();

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
	Adapter a = gc.getAdapter();

	FontMetrics fm = gc.getGraphicDestination().getFontMetrics(stringFont);
	String mess = (String) e.getProperty("message");


	int evtx = a.getX(e);
	int evty = a.getY(e);
	int evtLenght = fm.stringWidth(mess)+4;
	int evtHeight = fm.getHeight();

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
    public static Font stringFont = new Font("SansSerif", Font.BOLD, 12); 
    private Font backupFont;
 
}
