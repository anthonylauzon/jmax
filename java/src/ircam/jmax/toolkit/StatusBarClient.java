package ircam.jmax.toolkit;

import com.sun.java.swing.ImageIcon;

/**
 * The objects that use the StatusBar must implement this
 * interface */
public interface StatusBarClient {
  /**
   * The name that will be shown in the status bar. */
  abstract public String getName();
  /**
   * The icon that will be shown in the status bar.
   * Return null if this client has no icons. */
  abstract public ImageIcon getIcon();
}
