package ircam.jmax.editors.explode;

import java.awt.*;

/**
 * the status bar
 */
public interface StatusBar {

  /** write a message in the status bar
   */
  abstract public void post(StatusBarClient theClient, String message);

  /**
   * returns the size on screen
   */
  abstract public Dimension getSize();
}
