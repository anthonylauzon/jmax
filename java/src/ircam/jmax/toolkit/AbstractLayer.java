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

package ircam.jmax.toolkit;

import java.awt.*;

/** A simple implementation of the Layer interface, with a default behaviour
 * for the render method with Rectangle argument.
 * See the Layer interface for a documentation on the Layer mechanism. 
 * @see Layer 
 * @see RenderManager*/
public abstract class AbstractLayer implements Layer {
 
 public abstract void render(Graphics g, int order);

  /**
   * The default implementation just ignores the rectangle */
 public void render(Graphics g, Rectangle r, int order)
  {
    render(g, order);
  }

  /**
   * The default implementation returns null */
  public ObjectRenderer getObjectRenderer()
  {
    return null;
  }
}

