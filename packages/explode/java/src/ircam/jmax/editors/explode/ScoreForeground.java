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

package ircam.jmax.editors.explode;

import ircam.jmax.toolkit.*;

import java.awt.*;
import java.util.Enumeration;

/**
 * The foreground layer of a score representation */
public class ScoreForeground implements Layer {

  public ScoreForeground(ExplodeGraphicContext theGc)
  {
    gc = theGc;
    itsEventRenderer = new PartitionEventRenderer(gc);
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
    ScrEvent temp;

    Rectangle tempr = (Rectangle) g.getClip();

    
    g.clipRect(ScoreBackground.KEYEND, 0, gc.getGraphicDestination().getSize().width-ScoreBackground.KEYEND, gc.getGraphicDestination().getSize().height);
    
    for (Enumeration e = gc.getRenderManager().objectsIntersecting( r.x, r.y, r.width, r.height); e.hasMoreElements();) 
      {
	temp = (ScrEvent) e.nextElement();
	itsEventRenderer.render( temp, g, gc.getSelection().isInSelection(temp), gc);
      }

    g.setClip(tempr);
  }

  /** Returns the current EventRenderer */
  public ObjectRenderer getObjectRenderer()
  {
    return itsEventRenderer;
  }


  //--- Fields
  ExplodeGraphicContext gc;
  ObjectRenderer itsEventRenderer;

}
