//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell
// 
// This program may be used and distributed under the terms of the 
// accompanying LICENSE.
//
// This program is distributed WITHOUT ANY WARRANTY. See the LICENSE
// for DISCLAIMER OF WARRANTY.
// 
//
package ircam.jmax.toolkit;

import java.io.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;

import ircam.jmax.*;

/** Cache for the Icons; now handle the path */

public class IconCache {
  private static Hashtable table = new Hashtable();

  public static ImageIcon getIcon( String iconName)
  {
    ImageIcon image = (ImageIcon)table.get( iconName);

    if (image == null)
      {
	String path = MaxApplication.getProperty( "root" ) + File.separator + "images" + File.separator;

	image =  new ImageIcon( path + iconName);
	table.put( iconName, image);
      }

    return image;
  }
}


