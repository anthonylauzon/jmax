package ircam.jmax.editors.patcher;

import java.awt.*;

/**
 * The interface of all the object to paint in the sketch.
 */

public interface ErmesDrawable {
  abstract public void paint(Graphics g);
  abstract public void redraw();
  abstract public boolean intersects(Rectangle r);
}
