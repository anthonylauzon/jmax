 //
// jMax
// Copyright (C) 1994, 1995, 1998, 1999 by IRCAM-Centre Georges Pompidou, Paris, France.
// 
// This program is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public License
// as published by the Free Software Foundation; either version 2.1
// of the License, or (at your option) any later version.
// 
// See file COPYING.LIB for further informations on licensing terms.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
// 

package ircam.jmax.guiobj;

import java.awt.*;
import java.awt.event.*;
import java.util.*;

import ircam.jmax.editors.patcher.*;

import ircam.jmax.fts.*;

//
// The graphic inlet contained in subpatchers
//

public class Inlet extends InOutlet {
  public Inlet( FtsGraphicObject theFtsObject) 
  {
    super( theFtsObject);
  }

  public void drawTriangle( Graphics g, int x, int y, int w, int h) 
  {
    Color color = g.getColor();
    int offset = 2 * h/7;
      
    g.setColor( color.brighter());
    g.drawLine( x + h - offset, y + offset, x + h/2, y + h - offset);
      
    g.setColor( color.darker());
    g.drawLine( x + offset, y + offset, x + h - offset, y + offset);
    g.drawLine( x + offset, y + offset, x + h/2 - 1, y + h - offset - 1);
  }
}
