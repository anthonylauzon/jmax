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

package ircam.jmax.toolkit;


import java.util.*;
import java.awt.*;

import ircam.jmax.*;

/**
 * An handy base class implementing the RenderManager interface. It handles
 * the layers adding/removing, and the invocation of each Layer in turn on the 
 * render() calls.
 * @see RenderManager
 */
public class AbstractRenderer implements RenderManager {
  
  /** 
   * render all the layers in turn */
  public void render(Graphics g)
  {
    Layer aLayer;

    for (int i = 0; i< itsLayers.size(); i++)
      {
	aLayer = (Layer) itsLayers.elementAt(i);
	aLayer.render(g, i);
      }
  }


  /**
   * render the objects in the given rectangle */
  public void render(Graphics g, Rectangle r)
  {
    Layer aLayer;

    for (int i = 0; i< itsLayers.size(); i++)
      {
	aLayer = (Layer) itsLayers.elementAt(i);
	aLayer.render(g, r, i);
      }
  }

  /** add a layer */
  public void addLayer(Layer l)
  {
    itsLayers.addElement(l);
  }

  /** remove a layer */
  public void removeLayer(Layer l)
  {
    itsLayers.removeElement(l);
  }


  /**
   * returns its (current) event renderer
   */
  public ObjectRenderer getObjectRenderer() 
  {
    return null;
  }

  /**
   * returns the events whose graphic representation contains
   * the given point.
   */
  public Enumeration objectsContaining(int x, int y) 
  {
    return null;
  }

 /**
   * returns the first event containg the given point */
  public Object firstObjectContaining(int x, int y)
  {
    return null;
  }

  /**
   * returns an enumeration of all the events whose graphic representation
   * intersects the given rectangle.
   */
  public Enumeration objectsIntersecting(int x, int y, int w, int h) 
  {
    return null;
  }


  //--- Fields
  protected MaxVector itsLayers = new MaxVector();
}
