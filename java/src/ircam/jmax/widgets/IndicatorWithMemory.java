//
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU General Public License
// as published by the Free Software Foundation; either version 2
// of the License, or (at your option) any later version.
// 
// See file LICENSE for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
// 
// You should have received a copy of the GNU General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 
// Based on Max/ISPW by Miller Puckette.
//
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
    add( Box.createRigidArea(new Dimension(7,0)));
    add( indicatorLed);
  }

  public Dimension getMinimumSize()
  {
    FontMetrics fm = label.getFontMetrics( label.getFont());
    Dimension ldim = indicatorLed.getPreferredSize();
    int width = fm.stringWidth( label.getText()) + ldim.width;
    int height = Math.max( fm.getMaxAscent() + fm.getMaxDescent(), ldim.height);

    return new Dimension( width + 2 + 7, height + 2);
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

