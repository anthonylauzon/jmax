//
// jMax
// Copyright (C) 1999 by IRCAM
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
// Authors: Maurizio De Cecco, Francois Dechelle, Enzo Maggi, Norbert Schnell.
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
