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



