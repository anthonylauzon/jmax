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

  public void paint( Graphics g)
  {
    g.setColor( getForeground());
    g.drawRect( 0, 0, size - 1, size - 1);

    if ( state == ON)
      {
	int xState = (size - sizeState) / 2;

	g.setColor( onColor);
	g.fillRect( xState, xState, sizeState, sizeState);
      }
    else if ( memory == ON)
      {
	int xMemory = (size - sizeMemory) / 2;

	g.setColor( onColor);
	g.fillRect( xMemory, xMemory, sizeMemory, sizeMemory);
      }
  }

  protected static final int size = 12;
  protected static final int sizeState = 8;
  protected static final int sizeMemory = 4;

  protected int memory;
}

