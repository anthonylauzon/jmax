//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
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
	itsEventRenderer.render( temp, g, gc.getSelection().isInSelection(temp));
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
