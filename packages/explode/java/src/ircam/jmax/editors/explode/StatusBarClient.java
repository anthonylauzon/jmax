package ircam.jmax.editors.explode;

import com.sun.java.swing.ImageIcon;

/**
 * The objects that use the StatusBar must implement this
 * interface */
public interface StatusBarClient {
  abstract public String getName();
  abstract public ImageIcon getIcon();
}
