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

package ircam.jmax.editors.sequence.track;

import ircam.jmax.editors.sequence.*;
import ircam.jmax.toolkit.*;

import java.awt.*;

/**
 * The piano-roll event renderer in a Score with an ambitus: the line-based event, 
 * with a lenght , variable width, black color, a label.
 */
public class LogicEventRenderer implements ObjectRenderer {

  public LogicEventRenderer()
    {
    } 

  /**
   * constructor.
   */
  public LogicEventRenderer(SequenceGraphicContext theGc) 
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
    TrackEvent e = (TrackEvent) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int x = gc.getAdapter().getX(e) - LOGIC_XDIAMETER/2;
    int y = gc.getGraphicDestination().getSize().height/2 - LOGIC_YDIAMETER/2;


    //internal oval
    g.setColor(Color.lightGray);
    g.fillOval(x+1, y+1, LOGIC_XDIAMETER-2, LOGIC_YDIAMETER-2);

    if (selected) 
	g.setColor(Color.red);
    else 
	g.setColor(Color.blue);

    //external Oval
    g.drawOval(x, y, LOGIC_XDIAMETER, LOGIC_YDIAMETER);

    // the little 'l'
    backupFont = g.getFont();

    g.setFont(LogicFont);
    g.drawString("L", x+6, y+LOGIC_YDIAMETER-5);

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
    TrackEvent e = (TrackEvent) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e)-LOGIC_XDIAMETER/2;
    int evty = gc.getGraphicDestination().getSize().height/2-LOGIC_YDIAMETER/2;


    return  (evtx<=x && (evtx+LOGIC_XDIAMETER >= x) && evty<=y && (evty+LOGIC_YDIAMETER) >= y);
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
    TrackEvent e = (TrackEvent) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e)-LOGIC_XDIAMETER/2;
    int evty = gc.getGraphicDestination().getSize().height/2 - LOGIC_YDIAMETER/2;

    eventRect.setBounds(evtx, evty, LOGIC_XDIAMETER, LOGIC_YDIAMETER);
    tempRect.setBounds(x, y, w, h);
    return  eventRect.intersects(tempRect);

  }

    public static LogicEventRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new LogicEventRenderer();

	return staticInstance;
    }

  //------------Fields
    final static int LOGIC_XDIAMETER = 20;
    final static int LOGIC_YDIAMETER = LOGIC_XDIAMETER-4;

    SequenceGraphicContext gc;
    public static LogicEventRenderer staticInstance;
    public static Font LogicFont = new Font("helvetica", Font.BOLD, 14); 
    private Font backupFont;
 
}
