//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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







