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

import java.awt.*;
import java.awt.event.*;
import java.io.*;
import java.lang.*;

import javax.swing.*;

public class MemoryLed extends Led {

  public MemoryLed()
  {
    memory = 0;

    addMouseListener( new MouseAdapter() {
      public void mouseClicked( MouseEvent e)
	{
	  reset();
	}
      });
  }

  public void setState( int state)
  {
    if ( state == Led.ON)
      memory = Led.ON;

    super.setState( state);
  }

  public Dimension getPreferredSize()
  {
    return new Dimension( size, size);
  }

  public void reset()
  {
    if ( state == OFF)
      {
	memory = OFF;
	repaint();
      }
  }

  public void paintComponent( Graphics g)
  {
    Insets insets = getInsets();
    int w = getWidth() - insets.left - insets.right;
    int h = getHeight() - insets.top - insets.bottom;

    g.setColor( getBackground());
    g.fillRect( insets.left, insets.top, w, h);

    g.setColor( onColor);
    if ( state == ON)
      {
	g.fillRect( insets.left, insets.top, w, h);
      }
    else if ( memory == ON)
      {
	g.fillRect( insets.left + 2, insets.top + 2, w - 4, h - 4);
      }
  }

  protected static final int size = 12;

  protected int memory;
}

