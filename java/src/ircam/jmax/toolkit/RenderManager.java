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
import java.util.Enumeration;

/**
 * The controller of the render process.
 * The process of rendering is split in several graphic "planes"
 * called in turn. A plane is associated with a "priority", that is,
 * an order in the repaint chain. 0 is the first (background) layer.
 * A RenderManager offers a visibility over the geometric properties 
 * of the set of object to represent (objectIntersect, objectContaining methods).
 * A note about the object's representation:
 * Even if the rendering of a single object can be splitted into incremental
 * graphic operations performed by indipendent layers, it is often
 * usefull to have a ObjectRenderer that is able to represent an object
 * in its entirety (example: dynamic dragging of an object).
 */
public interface RenderManager {
  
  /**
   * render all the currently visible objects. */
  abstract public void render(Graphics g);

  /**
   * render the objects in the given rectangle */
  abstract public void render(Graphics g, Rectangle r);

  /**
   * add a layer */
  abstract public void addLayer(Layer l);

  /**
   * remove the given layer */
  abstract public void removeLayer(Layer l);

  /**
   * returns the enumeration of objects whose graphic representation contains
   * the given point.
   */
  abstract public Enumeration objectsContaining(int x, int y);

  /**
   * reduced version of the objectsContaining method, returns just the first
   * object containing a point. The exact meaning of "first" depends on the
   * specific implementation */
  abstract public Object firstObjectContaining(int x, int y);

  /**
   * returns an enumeration of all the objects whose graphic representation
   * intersects the given rectangle
   */
  abstract public Enumeration objectsIntersecting(int x, int y, int width, int height);


  /** Returns the render currently used to represent the objects. 
   * If this manager does not handle objects, or if the objects cannot be
   * represented entirely by an objectRenderer (for example, its representation
   * spans over several planes) return null */
  public abstract ObjectRenderer getObjectRenderer();
}



