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

package ircam.jmax.editors.bpf.renderers;

import ircam.jmax.toolkit.*;
import ircam.jmax.editors.bpf.*;

import java.awt.*;
import java.util.Enumeration;

/**
 * The foreground layer of a monodimensionalTrack representation */
public class BpfForeground implements Layer {

  public BpfForeground(BpfGraphicContext theGc)
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
      BpfPoint temp;
      Dimension d = gc.getGraphicDestination().getSize();
      Rectangle tempr = (Rectangle) g.getClip();

      g.clipRect(0, 0, gc.getGraphicDestination().getSize().width, gc.getGraphicDestination().getSize().height);

      drawPhantom(g, r);

      for (Enumeration e = gc.getRenderManager().
	       objectsIntersecting( r.x, 0, r.width, d.height); e.hasMoreElements();) 
	  {
	      temp = (BpfPoint) e.nextElement();
	      PointRenderer.getRenderer().render( temp, g, gc.getSelection().isInSelection(temp), gc);
	  }

      //draw the first object out of rectangle left bound  
      temp = gc.getFtsObject().getPreviousPoint(gc.getAdapter().getInvX(r.x));
      if(temp != null) 
	  PointRenderer.getRenderer().render( temp, g, gc.getSelection().isInSelection(temp), gc);
      
      g.setClip(tempr);
  }
    
    

  private void drawPhantom(Graphics g, Rectangle r)
  {
      BpfPoint first = gc.getDataModel().getPointAt(0);
      BpfPoint last = gc.getDataModel().getLastPoint();
      BpfAdapter a = gc.getAdapter();

      if(first == null) return;

      if(first.getTime() > 0)
	  {
	      ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_ON);
	      ((Graphics2D)g).setStroke(dashed);
	      
	      g.setColor(Color.gray);
	      g.drawLine(a.getX((float)0), a.getY(last), a.getX(first), a.getY(first));
	      ((Graphics2D)g).setRenderingHint(RenderingHints.KEY_ANTIALIASING, RenderingHints.VALUE_ANTIALIAS_OFF);
	      ((Graphics2D)g).setStroke(normal);
	  }
  }
  /** Returns the current EventRenderer */
  public ObjectRenderer getObjectRenderer()
  {
      return null;
  }

  //--- Fields
  BpfGraphicContext gc;

  final static float dash[] = {1.0f, 5.0f};
  final static BasicStroke dashed = new BasicStroke(1.0f, 
						    BasicStroke.CAP_BUTT, 
						    BasicStroke.JOIN_MITER, 
						    10.0f, dash, 2.0f);
  final static BasicStroke normal = new BasicStroke(1.0f, 
						    BasicStroke.CAP_BUTT, 
						    BasicStroke.JOIN_MITER, 
						    10.0f);
}


