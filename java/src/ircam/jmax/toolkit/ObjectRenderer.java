//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
// 

package ircam.jmax.toolkit;

import java.awt.*;

/**
 * The interface of the renderers of single objects in a pane.
 * No assumption are made on the nature of the object; this
 * structure is intended to work with object coming directly from
 * a data model, that is not forced to expose special graphic properties.
 * The strategy for rendering these object is up to the implementers:
 * either the object is a self-contained graphic object that knows how
 * to represent itself, or all the needed graphic parameters 
 * are obtained using specialized Adapters.
 * A basic ObjectRenderer offer a certain degree of visibility over the 
 * geometry of the object's representation, via the contains() and touches()
 * methods. Derived interfaces could enrich this set.
 * @see ircam.jmax.editors.explode.Adapter
 * @see ircam.jmax.editors.explode.PartitionEventRenderer
 */
public interface ObjectRenderer {

  /**
   * renders an object graphically, given an (awt) graphic context
   */
  public abstract void render(Object obj, Graphics g, boolean selected);
  
  /**
   * returns true if the representation of given object contains the given (graphic) point
   */
  public abstract boolean contains(Object obj, int x, int y);

  /**
   * returns true if the representation of the given object "touches" the given rectangle
   */
  public abstract boolean touches(Object obj, int x, int y, int w, int h);

}







