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


/**
 * A single graphic layer.
 * The process of rendering is split in several graphic Layers
 * called in turn. This allows a RenderManager to implement different politics
 * of paint (backgrounds, doubleBuffering...)
 * @see RenderManager
 */
public interface Layer {
  
  /**
   * Render all the visible object in this layer. The order parameter
   * specifies the current order in the rendering process. 
   * This information can be used by classes that implement different
   * layers at the same time. */
  public abstract void render(Graphics g, int order);

  /**
   * Render all the objects in the given rectangle. The 'order' parameter
   * specifies the current phase in the rendering process. 
   * This information can be used by classes that implement different
   * layers at the same time, or layers that change their behaviour
   * according to the order of calling */
  public abstract void render(Graphics g, Rectangle r, int order);


  /** Returns the render currently used to represent the objects in this
   * layer. If the layer does not handle objects (ex. backgrounds) return null */
  public abstract ObjectRenderer getObjectRenderer();

}


