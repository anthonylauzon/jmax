package ircam.jmax.editors.ermes;

import java.awt.Graphics;

/**
 * The interface of all the object to paint in the sketch.
 * It handles the dirty flag (is to repaint?)
 */
public interface ErmesDrawable {
  abstract public void Paint(Graphics g);
}
