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
package ircam.jmax.widgets;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

abstract public class Led extends JComponent {

  public static final int OFF = 0;
  public static final int ON = 1;

  public Led()
  {
    this(  Color.red);
  }

  public Led( Color onColor)
  {
    this.state = OFF;
    this.onColor = onColor;
  }

  public void setOnColor( Color color)
  {
    onColor = color;
  }

  public Color getOnColor()
  {
    return onColor;
  }

  public void setState( int state)
  {
    this.state = state;
    repaint();
  }

  public void setState( boolean state)
  {
    setState( state ? ON : OFF);
  }

  public int getState()
  {
    return state;
  }

  public Dimension getMinimumSize()
  {
    return getPreferredSize();
  }

  public Dimension getMaximumSize()
  {
    return getPreferredSize();
  }

  protected Color onColor;
  protected int state;
}
