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
 * An CueValue event renderer. It is represented as a vertical black bar
 */
public class CueEventRenderer implements ObjectRenderer {

  public CueEventRenderer()
    {
    } 

  /**
   * constructor.
   */
  public CueEventRenderer(SequenceGraphicContext theGc) 
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

    int x = gc.getAdapter().getX(e);
    int heigth = CUE_DIAMETER;
    int y =  gc.getGraphicDestination().getSize().height/2 - CUE_DIAMETER;
    int length = CUE_DIAMETER; //fixed length


    if (selected) 
	g.setColor(Color.red);
    else 
	g.setColor(Color.black);

    //event's Oval
    g.drawOval(x, y+CUE_DIAMETER/2, length, heigth);
    g.setColor(Color.white);
    g.fillOval(x+1, y+CUE_DIAMETER/2+1, length-2, heigth-2);

    tempFont = g.getFont();
    g.setFont(cueFont);
    g.setColor(Color.black);
    g.drawString(""+e.getProperty("integer"), x+5, y+CUE_DIAMETER+ERROR);
    g.setFont(tempFont);
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

    int evtx = gc.getAdapter().getX(e);
    int evty =  gc.getGraphicDestination().getSize().height/2 - CUE_DIAMETER;

    return  (evtx<=x && (evtx+CUE_DIAMETER >= x) && evty<=y && (evty+CUE_DIAMETER) >= y);
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
    TrackEvent e = (TrackEvent) obj;
    SequenceGraphicContext gc = (SequenceGraphicContext) theGc;

    int evtx = gc.getAdapter().getX(e);
    int evtheigth = CUE_DIAMETER;
    int evty = gc.getGraphicDestination().getSize().height/2 - CUE_DIAMETER;
    int evtlenght = CUE_DIAMETER;

    eventRect.setBounds(evtx, evty, evtlenght, evtheigth);
    tempRect.setBounds(x, y, w, h);
    return  eventRect.intersects(tempRect);

  }

    public static CueEventRenderer getRenderer()
    {
	if (staticInstance == null)
	    staticInstance = new CueEventRenderer();

	return staticInstance;
    }

  //------------Fields
    final static int ERROR = 6;
    final static int CUE_DIAMETER = 25;
    SequenceGraphicContext gc;
    public static CueEventRenderer staticInstance;
    private static Font cueFont = new Font("Helvetica", Font.BOLD, 14);
    private Font tempFont;
}
