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

package ircam.jmax.editors.patcher;

import java.awt.*;
import java.awt.event.*;

import ircam.jmax.editors.patcher.objects.*;

import javax.swing.*;
import javax.swing.text.*;
import javax.swing.event.*;

//
// The edit field contained in the editable objects (Message, Standard).
// That show an icon.
//

public class IconRenderer implements ObjectRenderer
{
  Icon icon;
  private Editable owner;

  //--------------------------------------------------------
  // CONSTRUCTOR
  //--------------------------------------------------------

  public IconRenderer(Editable object, Icon i)
  {
    super();

    owner = object;
    icon = i;
  }

  public void update()
  {
  }

  public boolean canResizeWidthTo(int width)
  {
    return width ==  icon.getIconWidth();
  }

  public int getHeight()
  {
    return icon.getIconHeight();
  }

  public int getWidth()
  {
    return icon.getIconWidth();
  }

  public void setBackground(Color color)
  {
  }

  public void prepareToRender()
  {
  }

  public void render(Graphics g, int x, int y, int w, int h)
  {
    icon.paintIcon(owner.getSketchPad(), g, x, y);
  }
}

