
package ircam.jmax.toolkit;

import java.awt.*;

/** An empty implementation of the Layer interface.
 * @see Layer for a description of the methods*/
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
