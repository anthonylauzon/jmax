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

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.border.*;

import java.awt.*;
import java.awt.event.*;
import java.lang.*;
import java.util.*;
import java.io.*;

public class IndicatorWithMemory extends JPanel {

  boolean value = false;

  public IndicatorWithMemory( String name)
  {
    this( name, null);
  }

  public IndicatorWithMemory( String name, String toolTipText)
  {
    setBorder( new EmptyBorder( 1, 1, 1, 1));
    setLayout( new BoxLayout( this, BoxLayout.X_AXIS));
    setToolTipText( toolTipText);

    label = new JLabel( name);
    label.setAlignmentX( LEFT_ALIGNMENT);
    label.setAlignmentY( CENTER_ALIGNMENT);

    indicatorLed = new MemoryLed();
    indicatorLed.setAlignmentX( RIGHT_ALIGNMENT);
    indicatorLed.setAlignmentY( CENTER_ALIGNMENT);

    add( label);
    add( Box.createHorizontalGlue());
    add( indicatorLed);
  }

  public Dimension getMinimumSize()
  {
    FontMetrics fm = label.getFontMetrics( label.getFont());
    Dimension ldim = indicatorLed.getPreferredSize();
    int width = fm.stringWidth( label.getText()) + ldim.width;
    int height = Math.max( fm.getMaxAscent() + fm.getMaxDescent(), ldim.height);

    return new Dimension( width + 2, height + 2);
  }

  public Dimension getPreferredSize()
  {
    return getMinimumSize();
  }

  public void setValue( boolean value)
  {
    indicatorLed.setState( value);
  }

  protected MemoryLed indicatorLed;
  protected JLabel label;
}

