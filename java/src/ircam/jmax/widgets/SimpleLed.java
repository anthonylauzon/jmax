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
package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

public class SimpleLed extends Led {

  public Dimension getPreferredSize()
  {
    return new Dimension( 14, 14);
  }

  public void paint( Graphics g)
  {
    int size = getSize().width;

    g.setColor( getForeground());
    g.drawRect( 0, 0, size - 1, size - 1);

    if ( state == ON)
      {
	g.setColor( onColor);
	g.fillRect( 2, 2, size - 4, size - 4);
      }
  }
}

