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

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.sequence.*;
import ircam.jmax.editors.sequence.track.*;

import java.awt.*;
import java.util.Enumeration;

/**
 * The foreground layer of a monodimensionalTrack representation */
public class MonoTrackForeground implements Layer {

  public MonoTrackForeground(SequenceGraphicContext theGc)
  {
    gc = theGc;
  }

  Rectangle tempRect = new Rectangle();
  /**
   * render all the visible objects in this layer (all the notes in the window)
   */
  public void render(Graphics g, int order) 
  {   
    tempRect.setBounds(0,0,gc.getGraphicDestination().getSize().width, 
		gc.getGraphicDestination().getSize().height);

    render(g, tempRect, order);
  }

  /** Layer interface */
  public void render(Graphics g, Rectangle r, int order)
  {
    TrackEvent temp;
    Dimension d = gc.getGraphicDestination().getSize();
    Rectangle tempr = (Rectangle) g.getClip();

    g.clipRect(ScoreBackground.KEYEND, 0, gc.getGraphicDestination().getSize().width-ScoreBackground.KEYEND, gc.getGraphicDestination().getSize().height);

    for (Enumeration e = gc.getRenderManager().
	     objectsIntersecting( r.x, 0, r.width, d.height); e.hasMoreElements();) 
	{
	    temp = (TrackEvent) e.nextElement();
	    temp.getRenderer().render( temp, g, gc.getSelection().isInSelection(temp), gc);
	}

    //draw the first object out of rectangle left bound  
    if(((MonoDimensionalAdapter)gc.getAdapter()).getViewMode() != MonoTrackEditor.PEAKS_VIEW)
	{
	    temp = ((FtsTrackObject)gc.getDataModel()).getPreviousEvent(gc.getAdapter().getInvX(r.x));
	    if(temp != null) 
		temp.getRenderer().render( temp, g, gc.getSelection().isInSelection(temp), gc);
	}
	
    g.setClip(tempr);
  }

  /** Returns the current EventRenderer */
  public ObjectRenderer getObjectRenderer()
  {
      return null;
  }

  //--- Fields
  SequenceGraphicContext gc;
}


