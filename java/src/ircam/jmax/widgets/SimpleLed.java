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

