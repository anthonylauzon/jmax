package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * The interface of the renderers of the event in the pane.
 */
public interface EventRenderer {

  /**
   * renders a score event graphically, given a given graphic context (ex a panel)
   */
  public abstract void render(ScrEvent e, Graphics g, boolean selected);
  
  /**
   * returns true if the given event contains the given (graphic) point
   */
  public abstract boolean contains(ScrEvent theEvent, int x, int y);
}

