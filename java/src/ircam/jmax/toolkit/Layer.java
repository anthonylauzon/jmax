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


