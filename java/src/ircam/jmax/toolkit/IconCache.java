package ircam.jmax.toolkit;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import com.sun.java.swing.*;

public class IconCache {
  private static Hashtable table = new Hashtable();

  public static ImageIcon getIcon( String filePath)
  {
    ImageIcon image = (ImageIcon)table.get( filePath);

    if (image == null)
      {
	image =  new ImageIcon( filePath);
	table.put( filePath, image);
      }

    return image;
  }
}
