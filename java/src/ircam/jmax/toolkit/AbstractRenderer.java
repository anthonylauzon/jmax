
package ircam.jmax.toolkit;

import ircam.jmax.utils.*;

import java.util.*;
import java.awt.*;

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
