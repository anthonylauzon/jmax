//
// jMax
// 
// Copyright (C) 1999 by IRCAM
// All rights reserved.
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

/** Cache/data base for the Icons; now handle the path */

public class Icons
{
  private static Hashtable table = new Hashtable();

  public static ImageIcon get(String iconName)
  {
    return (ImageIcon)table.get( iconName);
  }

  public static void loadIcon(String iconName, String fileName)
  {
    table.put(iconName, new ImageIcon(fileName));
  }
}


