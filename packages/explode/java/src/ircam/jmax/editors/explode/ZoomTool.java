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

import java.awt.event.*;
import java.awt.*;
import javax.swing.ImageIcon;

/**
 * The tool used to zoom in a given region of the score.
 * It uses just one user-interaction module:
 * a mouse tracker to choose the position.
 */ 
public class ZoomTool extends Tool implements PositionListener {

  /** constructor */
  public ZoomTool(ImageIcon theImageIcon) 
  {
    super("zoomer", theImageIcon);

    itsMouseTracker = new MouseTracker(this);
  }


  /**
   * the default interaction module for this tool
   */
  public InteractionModule getDefaultIM() 
  {
    return itsMouseTracker;
  }


  /**
   * called when this tool is "unmounted"
   */
  public void deactivate() 
  {
  }

  
  /**
   * PositionListener interface
   */
  public void positionChoosen(int x, int y, int modifiers) 
  {
 
    ExplodeGraphicContext egc = (ExplodeGraphicContext) gc;

    if ((modifiers & InputEvent.ALT_MASK) == 0) //zoom out
      {
	egc.getAdapter().setXZoom((int) (egc.getAdapter().getXZoom() * 100 * 2));

      }
    else 
      {                                       //zoom in
	if (egc.getAdapter().getXZoom() <= 0.01) return;
	egc.getAdapter().setXZoom((int) (egc.getAdapter().getXZoom() * 100 / 2));

      }

    //gc.getGraphicDestination().repaint();
  }
  

  //-------------- Fields

  MouseTracker itsMouseTracker;
}



